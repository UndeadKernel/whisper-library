#ifndef NETWORK_CONNECTOR
#define NETWORK_CONNECTOR

#include <string>
#include <vector>
#include <functional>
#include <map>

#include <genericpacket.hpp>
#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <pcapwrapper.hpp>
#include "socketSender.hpp"
#include <covertchannel.hpp>
#include <channelmanager.hpp>

using namespace std;

namespace whisper_library {

class NetworkConnector {
public:
	NetworkConnector(ChannelManager* channelmanager);
	~NetworkConnector();

	// Interface CovertChannel
	// Unterscheidung zwischen WIN32 und UNIX
	void sendTcp(string ip, TcpPacket packet);
	void sendUdp(string ip, UdpPacket packet);

	// Interface ChannelManager
	bool openConnection(string ip, CovertChannel* channel);
	void closeConnection(string ip);

	vector<string> adapters();
	unsigned int adapterCount();
	void setAdapter(string adapter_name);
	string adapterDescription(string adapter_name);

private:
	void retrievePacket();
	bool validIP(string ip);
	void addFilter(string ip, unsigned short port, string protocol);
	void removeFilter(string ip);
	void rebuildFilter();
	ChannelManager* m_channelmanager;
	PcapWrapper* m_pcap;
	SocketSender* m_socket;

	map<string, CovertChannel*> m_ip_mapping;

	unsigned int m_connection_count;
	string m_adapter;
	bool m_adapter_open;
	map<string, string> m_filter;
};

}

#endif // NETWORK_CONNECTOR