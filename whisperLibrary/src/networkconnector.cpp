#include "networkconnector.hpp"

namespace whisper_library {
	NetworkConnector::NetworkConnector(function<void(string, GenericPacket)> packet_received) {
		m_pcap = new PcapWrapper();
		m_socket = new SocketSender();

		m_adapter_open = false;
		m_connection_count = 0;
		m_packet_received = packet_received;
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
		if (!validIP(ip) || channel == NULL) {
			return false;
		}
		if (!m_adapter_open) {
			if (m_pcap->openAdapter(m_adapter.c_str(), m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
				return false;
			}
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

	void NetworkConnector::retrievePacket() {
		vector<bool> packet_data;
		GenericPacket generic_packet;
		while (m_adapter_open) {
			packet_data = m_pcap->retrievePacketAsVector(m_adapter.c_str());
			if (!packet_data.empty()) {
				generic_packet.setContent(packet_data);
				// TODO: get sender ip, split packet
				string ip;
				m_packet_received(ip, generic_packet);
			}
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