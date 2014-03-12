#include "networkconnector.hpp"
#include <iostream>

namespace whisper_library {
	NetworkConnector::NetworkConnector(function<void(string, GenericPacket)> packet_received) {
		m_pcap = new PcapWrapper();
		m_socket = new SocketSender();

		m_adapter_open = false;
		m_connection_count = 0;
		m_packet_received = packet_received;
		m_adapter = "";
	}

	NetworkConnector::~NetworkConnector() {
		delete m_pcap;
		delete m_socket;
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
	bool NetworkConnector::openConnection(string ip, CovertChannel* channel) {
		if (!validIP(ip) || channel == NULL || m_adapter.compare("") == 0) {
			return false;
		}
		if (!m_adapter_open) {
			if (m_pcap->openAdapter(m_adapter.c_str(), m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
				return false;
			}
			m_adapter_open = true;
			m_connection_count++;
			std::thread packet_receiver(std::bind(&NetworkConnector::retrievePacket, this));
			packet_receiver.detach();
		}
		else {
			m_connection_count++;
		}
		addFilter(ip, channel->port(), channel->protocol());
		return true;
	}

	void NetworkConnector::closeConnection(string ip) {
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
		cout << "filter: " << complete_filter << endl;
		m_pcap->applyFilter(m_adapter.c_str(), complete_filter.c_str());
	}

	struct IpHeader{
		unsigned char        ip_vhl;
		unsigned char        service;
		unsigned short       length;
		unsigned short       identification;
		u_short              offset;
		u_char               ttl;
		u_char               protocol;
		u_short              chksum;
		unsigned long        src;
		unsigned long        dst;
	};	/*
#define IP_HL(ip)       (((ip)->ip_vhl) & 0x0f)#define IP_V(ip)        (((ip)->ip_vhl) >> 4)
*/
	vector<bool> NetworkConnector::toLittleEndian(vector<bool> big_endian) {
		vector<bool> little_endian;
		for (unsigned int i = 0; i < big_endian.size()-7; i = i+8) {
			vector<bool> byte;
			for (unsigned int j = 0; j < 8; j++) {
				byte.push_back(big_endian[i + (7-j)]);
			}
			little_endian.insert(little_endian.end(), byte.begin(), byte.end());
		}
		return little_endian;
	}

	void NetworkConnector::retrievePacket() {
		cout << "packet receiver started" << endl;
		vector<bool> packet_data;
		GenericPacket generic_packet;
		while (m_adapter_open) {
			packet_data = m_pcap->retrievePacketAsVector(m_adapter.c_str());
			if (!packet_data.empty()) {
				cout << "length: " << packet_data.size() / 8 << " byte" << endl;
				vector<bool> packet_little_endian = toLittleEndian(packet_data);
				for (unsigned int i = 0; i < packet_little_endian.size(); i++) {
					if (i % 4 == 0) {
						cout << " ";
					}
					cout << packet_little_endian[i];
				}
				cout << endl << endl;
				int buffer_size = packet_little_endian.size() / 8;
				unsigned char* buffer = static_cast<unsigned char*>(malloc(buffer_size));

				for (unsigned int i = 0; i < buffer_size; i++) {
					buffer[i] = 0; // initialize
					for (unsigned int j = 0; j < 8; j++) {
						buffer[i] |= (packet_little_endian[j + (i * 8)] ? 1 : 0) << (7-j);
					}
				}
				for (unsigned int i = 0; i < buffer_size; i++) {
					if (i % 8 == 0) {
						printf(" ");
					}
					if (i % 2 == 0) {
						printf(" ");
					}
					if (i % 16 == 0) {
						printf("\n");
					}
					printf("%.2X", buffer[i]);
				}
				IpHeader* ip_header = (IpHeader*)(buffer + 14);
				printf("Protokoll: %.2X", ip_header->protocol);
				free(buffer);
			}
		/*	whisper_library::PcapWrapper::PcapPacket p = m_pcap->retrievePacket(m_adapter.c_str());
			const u_char* data = p.payload;
			if (data != NULL) {
				cout << "received packet: " << endl;
				printf("%.2", *data);
				//generic_packet.setContent(packet_data);
				// TODO: get sender ip, split packet
				string ip = "";*/

			/*	for (unsigned int i = 0; i < strlen(data); i++) {
					if (i % 8 == 0) {
						printf(" ");
					}
					if (i % 2 == 0) {
						printf(" ");
					}
					if (i % 16 == 0) {
						printf("\n");
					}
					printf("%.2X", data[i]);
				}*/

			//	m_packet_received(ip, generic_packet);

			/*	whisper_library::PcapWrapper::PcapPacket p = m_pcap->retrievePacket(m_adapter.c_str());
				const u_char* data = p.payload;
				ip_header* ip_h;
				TcpPacket tcp;
				unsigned int ip_length = 0;
				if (data != NULL) {
					ip_h = ((struct ip_header*)(data + 14));
					ip_length = IP_HL(ip_h) * 4;
					//	tcp = ((whisper_library::TcpPacket*)(data + ip_length));
					// denk drann: Alle Daten da drinnen sind in Big En
				} 
			} */
		}
	}

	// Sending
	void NetworkConnector::sendTcp(string ip, TcpPacket packet) {
		#ifndef WIN32
			// UNIX
			int adapter_id = m_pcap->adapterId(m_adapter.c_str(), m_pcap->ADAPTER_NAME);
			if (adapter_id < 0) {
				return ;
			}
			vector<char *> addresses = m_pcap->adapterAddresses(adapter_id);
			// TODO get right address
			m_socket->sendTcp(addresses[0], ip, packet);
		#else
			// TODO: add ethernet and ip header
			m_pcap->sendPacket(m_adapter.c_str(), packet.packet());
		#endif

	}

	void NetworkConnector::sendUdp(string ip, UdpPacket packet) {
		m_socket->sendUdp(ip, packet);
	}

	// Utility
	bool NetworkConnector::validIP(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address::from_string(ip, ec);
		if (ec) {
			return false;
		}
		return true;
	}
}