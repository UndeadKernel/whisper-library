#include "networkconnector.hpp"
#include <iostream>
#include "ipheaderv4.hpp"
#include <boost/algorithm/string.hpp>
#include "ethernetheader.hpp"
#include <boost/asio.hpp>
#include <chrono>

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
			m_pcap_sender = new PcapWrapper();
		#endif
	}

	NetworkConnector::~NetworkConnector() {
		delete m_pcap;
		delete m_socket;

		#ifdef WIN32
			if(m_adapter_addresses) {
				free(m_adapter_addresses);
			}
			delete m_pcap_sender;
		#endif
	}

	// Adapter Handling
	void NetworkConnector::setAdapter(string adapter_name) {
		if (m_pcap->adapterId(adapter_name, m_pcap->ADAPTER_NAME) != -2 && !m_adapter_open) {
			m_adapter = adapter_name;
			vector<string> addresses = adapterAddresses();
			for (unsigned int i = 0; i < addresses.size(); i++) {
				if (validIPv4(addresses[i])) {
					m_source_ip = addresses[i];
				}
			}
		}
	}

	vector<string> NetworkConnector::adapters() {
		return m_pcap->adapterNames();
	}

	string NetworkConnector::adapterDescription(string adapter_name) {
		int adapter_id = m_pcap->adapterId(adapter_name, m_pcap->ADAPTER_NAME);
		if (adapter_id < 0) {
			return "";
		}
		return m_pcap->adapterDescription(adapter_id);
	}

	unsigned int NetworkConnector::adapterCount() {
		return m_pcap->adapterCount();
	}

	vector<string> NetworkConnector::adapterAddresses() {
		int adapter_id = m_pcap->adapterId(m_adapter, m_pcap->ADAPTER_NAME);
		return (adapter_id < 0 ? vector<string>() : m_pcap->adapterAddresses(adapter_id));
	}

	// Connection
	bool NetworkConnector::openListener(string ip, CovertChannel* channel) {
		if (!validIPv4(ip) || channel == NULL || m_adapter.compare("") == 0) {
			return false;
		}
		if (!m_adapter_open) {
			if (m_pcap->openAdapter(m_adapter, m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
				return false;
			}
			#ifdef WIN32
				if (m_pcap_sender->openAdapter(m_adapter, m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
					return false;
				}
			#endif
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
				m_adapter_open = false;
				m_pcap->closeAdapter(m_adapter);
				#ifdef WIN32
					m_pcap_sender->closeAdapter(m_adapter);
				#endif
			}
		}
	}

	void NetworkConnector::retrievePacket() {
		vector<bool> packet_data;
		while (m_adapter_open) {
			packet_data = m_pcap->retrievePacketAsVector(m_adapter);

			if (!packet_data.empty()) {
				thread packet_received(bind(&NetworkConnector::packetReceived, this, packet_data));
				packet_received.detach();
			}
		}
	}
	void NetworkConnector::packetReceived(vector<bool> packet_data) {
		GenericPacket generic_packet;
		vector<bool> packet_little_endian;
		unsigned int length_bit;
		vector<bool> application_layer;
		packet_little_endian = switchEndian(packet_data);

		IpHeaderv4 ip_header(packet_little_endian);
		length_bit = ip_header.ipHeaderLength() * 32;

		application_layer.insert(application_layer.begin(), packet_little_endian.begin() + length_bit + 112, packet_little_endian.end());
		generic_packet.setContent(application_layer);
		m_packet_received(ip_header.sourceIpDotted(), generic_packet);
	}

	// Capture Filter
	void NetworkConnector::addFilter(string ip, unsigned short port, string protocol) {
		string filter_rule = "src " + ip + " and port " + to_string(port) + " and " + protocol;
		m_filter.insert(pair<string, string>(ip, filter_rule));
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
		m_pcap->applyFilter(m_adapter, complete_filter);
	}

	// Sending
	void NetworkConnector::sendTcp(string ip, TcpPacket packet) {
		#ifdef WIN32
			if (!m_adapter_open) {
				return;
			}
			// WIN32
			vector<bool> frame;
			// ethernet header
			EthernetHeader ethernet_header;
			AdapterInfo adapter_info = win32GetAdapterInfo();
			ethernet_header.setSourceMAC(adapter_info.mac_address);
			string destination_mac = win32GetDestinationMAC(m_source_ip, ip, adapter_info);		
			if (destination_mac.compare("") == 0) {
				// no mac found
				return;
			}
			ethernet_header.setDestinationMAC(destination_mac);
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
			ip_header.setSourceIp(m_source_ip);
			ip_header.setDestinationIp(ip);
			ip_header.calculateChecksum();
			vector<bool> ip_header_bin = ip_header.toVector();
			frame.insert(frame.end(), ip_header_bin.begin(), ip_header_bin.end());

			// tcp header+body
			packet.calculateChecksum(boost::asio::ip::address_v4::from_string(m_source_ip).to_ulong(),
									 boost::asio::ip::address_v4::from_string(ip).to_ulong(), 0, 6);
			vector<bool> tcp = packet.packet();
			frame.insert(frame.end(), tcp.begin(), tcp.end());
		
			vector<bool> frame_big_endian = switchEndian(frame);
			m_pcap_sender->sendPacket(m_adapter, frame_big_endian);
		#else
			// UNIX
			m_socket->sendTcp(m_source_ip, ip, packet);
		#endif
	}

	void NetworkConnector::sendUdp(string ip, UdpPacket packet) {
		m_socket->sendUdp(ip, packet);
	}

	// Utility
	bool NetworkConnector::validIPv4(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
		return (!ec) && address.is_v4();
	}

	vector<bool> NetworkConnector::switchEndian(vector<bool> binary) {
		vector<bool> switched_endian;
		for (unsigned int i = 0; i < binary.size() - 7; i = i + 8) {
			vector<bool> byte;
			for (unsigned int j = 0; j < 8; j++) {
				byte.push_back(binary[i + (7 - j)]);
			}
			switched_endian.insert(switched_endian.end(), byte.begin(), byte.end());
		}
		return switched_endian;
	}

// Win32 only
#ifdef WIN32
	NetworkConnector::AdapterInfo NetworkConnector::win32GetAdapterInfo() {
		AdapterInfo values = { 0, NULL };
		if (m_adapter.empty()) { return values; }
		int i = 0;
		string adapter_name = m_adapter.substr(12, string::npos);
		PIP_ADAPTER_ADDRESSES current_addresses				= NULL;
		PIP_ADAPTER_ADDRESSES adapter_addresses				= NULL;
		PIP_ADAPTER_GATEWAY_ADDRESS_LH gateway_addresses	= NULL;
		unsigned long output_buffer_length					= 15000;
		SOCKADDR_IN* gateway_adress;
		
		// m_adapter_addresses unset
		if (!(m_adapter_addresses) ) {			
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
					fprintf(stderr, "Error: Buffer overflow. \n");
					adapter_addresses = NULL;
				} else { break;	}
			} while (i++ < maximum_tries && return_value == ERROR_BUFFER_OVERFLOW);

			if (return_value == NO_ERROR) {
				m_adapter_addresses = adapter_addresses;
			} else {
				fprintf(stderr, "Error(#%d): Failed to retrieve adapter addresses.\n", return_value);
				if (adapter_addresses) { free(adapter_addresses); }
				return values;
			}
		}
		// m_adapter_addresses set
		current_addresses = m_adapter_addresses;
		while (current_addresses) {
			if (strcmp(adapter_name.c_str(), reinterpret_cast<char*>(current_addresses->AdapterName)) == 0) { // equal
				if (current_addresses->PhysicalAddressLength != 0) {
					for (i = 0; i < static_cast<int>(current_addresses->PhysicalAddressLength); i++) {
						values.mac_address[i] = static_cast<unsigned char>(current_addresses->PhysicalAddress[i]);
					}
				}
				gateway_addresses = current_addresses->FirstGatewayAddress;
				while (gateway_addresses && !values.gateway_address) {
					if (gateway_addresses->Length > 0) {
						gateway_adress = (reinterpret_cast<SOCKADDR_IN*>(gateway_addresses->Address.lpSockaddr));
						switch (gateway_adress->sin_family) {
							case AF_INET: {
								values.gateway_address = (static_cast<IPAddr>(gateway_adress->sin_addr.S_un.S_addr));
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

	string NetworkConnector::win32GetDestinationMAC(string source_ip, string destination_ip, AdapterInfo adapter_info) {
		string destination_mac;
		try {
			destination_mac = m_destination_macs.at(destination_ip);
		}
		catch (out_of_range) {
			// mac not found in cache, send arp request
			destination_mac = win32SendArp(inet_addr(source_ip.c_str()), inet_addr(destination_ip.c_str()));
			if (destination_mac.compare("") == 0) { // equal
				// ip not found locally, get gateway mac
				if (adapter_info.gateway_address != 0) {
					destination_mac = win32SendArp(inet_addr(source_ip.c_str()), adapter_info.gateway_address);
				}
				else {
					fprintf(stderr, "No default gateway found.\n");
					return "";
				}
			}
			m_destination_macs.emplace(destination_ip, destination_mac);  // save in cache
		}
		return destination_mac;
	}

	string NetworkConnector::win32SendArp(IPAddr source_ip, IPAddr destination_ip) {
		DWORD return_value;
		ULONG adress_length = 6;
		unsigned char mac_address[6];

		return_value = SendARP(destination_ip, source_ip, mac_address, &adress_length);

		if (return_value == NO_ERROR) {
			return EthernetHeader::toMacString(mac_address);
		}
		else {
			return "";
		} 
	}
#endif
}
