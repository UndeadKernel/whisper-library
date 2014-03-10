#include "networkconnector.hpp"

namespace whisper_library {
	NetworkConnector::NetworkConnector(ChannelManager* channelmanager) {
		m_channelmanager = channelmanager;
		m_pcap = new PcapWrapper();
		m_socket = new SocketSender();

		m_adapter_open = false;
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

	// Connection
	bool NetworkConnector::openConnection(string ip, CovertChannel* channel) {
		if (!validIP(ip) || channel == NULL) {
			return false;
		}
		if (!m_adapter_open) {
			if (m_pcap->openAdapter(m_adapter.c_str(), m_pcap->DEFAULT_MAXPACKETSIZE, true, 1) == -1) {
				return false;
			}
			std::thread packet_receiver(std::bind(&NetworkConnector::retrievePacket, this));
			packet_receiver.detach();
		}
		addFilter(ip, channel->port(), channel->protocol());
		return true;
	}

	void NetworkConnector::addFilter(string ip, unsigned short port, string protocol) {
		string filter_rule = "dst " + ip + " and port " + to_string(port) + " and " + protocol;
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