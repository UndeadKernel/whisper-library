#include "networkconnector.hpp"
#include <iostream>
#include "ipheaderv4.hpp"
#include <boost/algorithm/string.hpp>
#include "ethernetheader.hpp"

namespace whisper_library {
	NetworkConnector::NetworkConnector(function<void(string, GenericPacket)> packet_received) {
		m_pcap = new PcapWrapper();
		m_socket = new SocketSender();

		m_adapter_open = false;
		m_connection_count = 0;
		m_packet_received = packet_received;
		m_adapter = "";
		#ifdef WIN32
		m_adapter_addresses = NULL;
		#endif
	}

	NetworkConnector::~NetworkConnector() {
		delete m_pcap;
		delete m_socket;
		#ifdef WIN32
		if(m_adapter_addresses) { (m_adapter_addresses); }
		#endif
	}

	// Adapter Handling
	void NetworkConnector::setAdapter(string adapter_name) {
		if (m_pcap->adapterId(adapter_name.c_str(), m_pcap->ADAPTER_NAME) != -2 && !m_adapter_open) {
			m_adapter = adapter_name;
		}
	}

	vector<string> NetworkConnector::adapters() {
		vector<char *> adapter_list;
		vector<string> adapter_list_string;
		adapter_list = m_pcap->adapterNames();
		for (vector<char*>::iterator it = adapter_list.begin(); it != adapter_list.end(); it++) {
			adapter_list_string.push_back(*it);
		}
		return adapter_list_string;
	}

	string NetworkConnector::adapterDescription(string adapter_name) {
		int adapter_id = m_pcap->adapterId(adapter_name.c_str(), m_pcap->ADAPTER_NAME);
		if (adapter_id < 0) {
			return "";
		}
		const char* description = m_pcap->adapterDescription(adapter_id);
		if (description == NULL) {
			return "";
		}
		else {
			return description;
		}
	}

	unsigned int NetworkConnector::adapterCount() {
		return m_pcap->adapterCount();
	}

	vector<string> NetworkConnector::adapterAddresses() {
		int adapter_id = m_pcap->adapterId(m_adapter.c_str(), m_pcap->ADAPTER_NAME);
		vector<string> string_addresses;
		if (adapter_id < 0) {
			return string_addresses;
		}
		vector<char*> addresses = m_pcap->adapterAddresses(adapter_id);
		for (vector<char*>::iterator it = addresses.begin(); it != addresses.end(); it++) {
			string_addresses.push_back(*it);
		}

		return string_addresses;
	}

	// Connection
	bool NetworkConnector::openListener(string ip, CovertChannel* channel) {
		if (!validIP(ip) || channel == NULL || m_adapter.compare("") == 0) {
			return false;
		}
		if (!m_adapter_open) {
			if (m_pcap->openAdapter(m_adapter.c_str(), m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
				return false;
			}
			m_adapter_open = true;
			m_connection_count++;
			thread packet_receiver(std::bind(&NetworkConnector::retrievePacket, this));
			packet_receiver.detach();
		}
		else {
			m_connection_count++;
		}
		addFilter(ip, channel->port(), channel->protocol());
		return true;
	}

	void NetworkConnector::closeListener(string ip) {
		if (m_adapter_open) {
			removeFilter(ip);
			m_connection_count--;
			if (m_connection_count == 0) {
				m_pcap->closeAdapter(m_adapter.c_str());
				m_adapter_open = false;
			}
		}
	}

	void NetworkConnector::addFilter(string ip, unsigned short port, string protocol) {
		string filter_rule = "src " + ip + " and port " + to_string(port) + " and " + protocol;
		m_filter.emplace(ip, filter_rule);
		rebuildFilter();
	}

	void NetworkConnector::removeFilter(string ip) {
		m_filter.erase(ip);
		rebuildFilter();
	}

	void NetworkConnector::rebuildFilter() {
		string complete_filter = "";
		for (map<string, string>::iterator it = m_filter.begin(); it != m_filter.end(); it++) {
			pair<string, string> element = *it;
			if (it == m_filter.begin()) {
				complete_filter += "(" + element.second + ")";
			}
			else {
				complete_filter += " or (" + element.second + ")";
			}
		}
		m_pcap->applyFilter(m_adapter.c_str(), complete_filter.c_str());
	}

	vector<bool> NetworkConnector::switchEndian(vector<bool> binary) {
		vector<bool> switched_endian;
		for (unsigned int i = 0; i < binary.size()-7; i = i+8) {
			vector<bool> byte;
			for (unsigned int j = 0; j < 8; j++) {
				byte.push_back(binary[i + (7-j)]);
			}
			switched_endian.insert(switched_endian.end(), byte.begin(), byte.end());
		}
		return switched_endian;
	}

	void NetworkConnector::retrievePacket() {
		vector<bool> packet_data;
		GenericPacket generic_packet;
		while (m_adapter_open) {
			packet_data = m_pcap->retrievePacketAsVector(m_adapter.c_str());

			if (!packet_data.empty()) {
				vector<bool> packet_little_endian = switchEndian(packet_data);

				IpHeaderv4 ip_header(packet_little_endian);
				unsigned int length_bit = ip_header.ipHeaderLength() * 32;

				vector<bool> application_layer;
				application_layer.insert(application_layer.begin(), packet_little_endian.begin() + length_bit + 112, packet_little_endian.end());
				generic_packet.setContent(application_layer);
				m_packet_received(ip_header.sourceIpDotted(), generic_packet);
			}
		}
	}

	// Sending
	void NetworkConnector::sendTcp(string ip, TcpPacket packet) {
		vector<string> addresses = adapterAddresses();
		string source_ip;
		for (unsigned int i = 0; i < addresses.size(); i++) {
			if(validIP(addresses[i])) {
				source_ip = addresses[i];
			}
		}
		#ifndef WIN32
			// UNIX
			m_socket->sendTcp(source_ip, ip, packet);
		#else
			vector<bool> frame;
			EthernetHeader ethernet_header;
			MAC_AND_GATEWAY mac_and_gateway = win32FetchMACAddressAndGateway();
			ethernet_header.setSourceMAC(mac_and_gateway.mac_address);	// 6 byte
			ethernet_header.setDestinationMAC(win32GetDestinationMAC(inet_addr(source_ip.c_str()),mac_and_gateway.gateway_address)); // 6 byte
			ethernet_header.setEthernetType(2048); // Ipv4
			vector<bool> ethernet_header_bin = ethernet_header.toVector();
			frame.insert(frame.end(), ethernet_header_bin.begin(), ethernet_header_bin.end());

			// ip header
			IpHeaderv4 ip_header;
			ip_header.setVersion(4);
			ip_header.calculateHeaderLength();
			ip_header.setTotalLength(ip_header.ipHeaderLength()*4 + packet.packet().size() / 8);
			ip_header.setTimeToLive(128);
			ip_header.setProtocol(IpHeaderv4::TCP);
			ip_header.setSourceIp(source_ip);
			ip_header.setDestinationIp(ip);
			ip_header.calculateChecksum();
			vector<bool> ip_header_bin = ip_header.toVector();
			frame.insert(frame.end(), ip_header_bin.begin(), ip_header_bin.end());

			// tcp
			vector<bool> tcp = packet.packet();
			frame.insert(frame.end(), tcp.begin(), tcp.end());
		
			vector<bool> frame_big_endian = switchEndian(frame);
			m_pcap->sendPacket(m_adapter.c_str(), frame_big_endian);
		#endif
	}

	void NetworkConnector::sendUdp(string ip, UdpPacket packet) {
		m_socket->sendUdp(ip, packet);
	}

	// Utility
	bool NetworkConnector::validIP(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address::from_string(ip, ec);
		return (ec ? false : true);
	}

// Win32 only
#ifdef WIN32
	NetworkConnector::MAC_AND_GATEWAY NetworkConnector::win32FetchMACAddressAndGateway() {
		MAC_AND_GATEWAY values = { 0, NULL };
		if (m_adapter.empty()) { return values; }
		int i = 0;
		string adapter_name = m_adapter.substr(12, string::npos);
		PIP_ADAPTER_ADDRESSES current_addresses				= NULL;
		PIP_ADAPTER_ADDRESSES adapter_addresses				= NULL;
		PIP_ADAPTER_GATEWAY_ADDRESS_LH gateway_addresses	= NULL;
		unsigned long output_buffer_length					= 15000;
		SOCKADDR_IN* gateway_adress;
		
		// m_adapter_addresses unset
		if (!m_adapter_addresses) {
			
			int maximum_tries  = 25;
			DWORD return_value = 0;
			do {
				adapter_addresses = static_cast<IP_ADAPTER_ADDRESSES *>(malloc(output_buffer_length));
				if (adapter_addresses == NULL) {
					fprintf(stderr, "Error: Out of memory.\n");
					return values;
				}
				return_value = GetAdaptersAddresses(static_cast<unsigned long>(AF_UNSPEC), GAA_FLAG_INCLUDE_GATEWAYS, NULL, adapter_addresses, &output_buffer_length);
				if (return_value == ERROR_BUFFER_OVERFLOW) {
					free(adapter_addresses);
					adapter_addresses = NULL;
				} else { break;	}
			} while (i++ < maximum_tries && return_value == ERROR_BUFFER_OVERFLOW);

			if (return_value == NO_ERROR) {
				m_adapter_addresses = &adapter_addresses;
			} else {
				fprintf(stderr, "Error(#%d): Failed to retrieve adapter addresses.\n", return_value);
				if (adapter_addresses) { free(adapter_addresses); }
				return values;
			}
		}

		// m_adapter_addresses set
		fprintf(stdout, "Assigned adapter name: %s\n", adapter_name.c_str());
		current_addresses = *m_adapter_addresses;
		while (current_addresses) {
			fprintf(stdout, "Adapter name: %s\n", reinterpret_cast<char*>(current_addresses->AdapterName));
			if (strcmp(adapter_name.c_str(), reinterpret_cast<char*>(current_addresses->AdapterName)) == 0) { // equal
				if (current_addresses->PhysicalAddressLength != 0) {
					fprintf(stdout, "MAC-Address: ");
					for (i = 0; i < static_cast<int>(current_addresses->PhysicalAddressLength); i++) {
						fprintf(stdout, ((i + 1) == static_cast<int>(current_addresses->PhysicalAddressLength) ? "%.2X\n" : "%.2X:"), static_cast<int>(current_addresses->PhysicalAddress[i]));
						values.mac_address[i] = static_cast<char>(current_addresses->PhysicalAddress[i]);
					}
				}
				gateway_addresses = current_addresses->FirstGatewayAddress;
				while (gateway_addresses && !values.gateway_address) {
					if (gateway_addresses->Length > 0) {
						gateway_adress = (reinterpret_cast<SOCKADDR_IN*>(current_addresses->FirstGatewayAddress->Address.lpSockaddr));
						switch (gateway_adress->sin_family) {
							case AF_INET: {
								values.gateway_address = (static_cast<IPAddr>(gateway_adress->sin_addr.S_un.S_addr));
								fprintf(stdout, "Gateway-Address: %d\n", static_cast<IPAddr>(gateway_adress->sin_addr.S_un.S_addr));
								break;
							}
							case AF_INET6: {
								// TODO: IPv6
								// Needs alternative SOCKADRR handling, so we could "outsource" it.
								break;
							}
						}
					}
					gateway_addresses = gateway_addresses->Next;
				}
				if (values.gateway_address) {
					break; // jump out of loop
				}
			}
			current_addresses = current_addresses->Next;
		}	
		return values; // there is a possible case where one of the struct fields >might< be empty if method is used on bonded devices. 
	}

	const char* NetworkConnector::win32GetDestinationMAC(IPAddr source_ip, IPAddr destination_ip) {
		DWORD return_value;
		char mac_address[6]; // 6 byte
		ULONG adress_length = 6;

		return_value = SendARP(destination_ip, source_ip, mac_address, &adress_length);

		if (return_value == NO_ERROR) {
			return mac_address;
		}
		else {
			return "";
		} 
	}
#endif
}