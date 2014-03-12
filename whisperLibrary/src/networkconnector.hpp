#ifndef NETWORK_CONNECTOR
#define NETWORK_CONNECTOR

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <thread>

#include <genericpacket.hpp>
#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <pcapwrapper.hpp>
#include "socketSender.hpp"
#include <covertchannel.hpp>

using namespace std;

namespace whisper_library {

class NetworkConnector {
public:
	NetworkConnector(function<void(string, GenericPacket)> packet_received);
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
	vector<string> adapterAddresses();

private:
	void retrievePacket();
	bool validIP(string ip);
	void addFilter(string ip, unsigned short port, string protocol);
	void removeFilter(string ip);
	void rebuildFilter();
	PcapWrapper* m_pcap;
	SocketSender* m_socket;



	unsigned int m_connection_count;
	string m_adapter;
	bool m_adapter_open;
	map<string, string> m_filter;
	function<void(string, GenericPacket)> m_packet_received;
};

}

#endif // NETWORK_CONNECTOR