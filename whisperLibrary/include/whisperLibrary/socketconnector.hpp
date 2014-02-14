#ifndef SOCKET_CONNECTOR
#define SOCKET_CONNECTOR

#include "common.hpp"
#include "channelmanager.hpp"

namespace whisper_library {

class SocketConnector {

public:
	SocketConnector(ChannelManager * channelmanager)
		: m_channelmanager(channelmanager) {};
	void openConnection(int protokoll);
	void sendTcpPacket(TcpPacket packet);
	void sendUdpPacket(UdpPacket packet);
private:
	ChannelManager* m_channelmanager;
};
}
#endif // SOCKET_CONNECTOR