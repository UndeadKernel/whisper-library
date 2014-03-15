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
		m_pcap->applyFilter(m_adapter.c_str(), complete_filter.c_str());
	}
	vector<bool> NetworkConnector::switchEndian(vector<bool> big_endian) {
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
		int adapter_id = m_pcap->adapterId(m_adapter.c_str(), m_pcap->ADAPTER_NAME);
		if (adapter_id < 0) {
			return;
		}
		vector<char *> addresses = m_pcap->adapterAddresses(adapter_id);
		string source_ip = "";
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
			// Ethernet - TODO find MAC address
			EthernetHeader ethernet_header;
			ethernet_header.setDestinationMAC("78:92:9c:2d:49:74"); // 6 byte
			ethernet_header.setSourceMAC("bc:5f:f4:5d:e9:c5");	// 6 byte
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
		if (ec) {
			return false;
		}
		return true;
	}
}