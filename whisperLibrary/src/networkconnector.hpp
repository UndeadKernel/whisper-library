#ifndef NETWORK_CONNECTOR
#define NETWORK_CONNECTOR

#include <string>
#include <vector>
#include <functional>
#include <genericpacket.hpp>
#include "tcppacket.hpp"
#include "udppacket.hpp"

using namespace std;

namespace whisper_library {

class NetworkConnector {
public:
	void sendTcp(TcpPacket packet);
	void sendUdp(UdpPacket packet);

	bool openConnection(string ip, unsigned short port, string protocol, function<void(GenericPacket)> callback_message_received);
	void closeConnection();

	vector<string> adapters();
	unsigned int adapterCount();
	void setAdapter(string adapter_name);
	string adapterDescription(string adapter_name);

private:
	void retrievePacket();
	string m_adapter;
};

}

#endif // NETWORK_CONNECTOR