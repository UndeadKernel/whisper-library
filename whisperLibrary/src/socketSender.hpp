#ifndef SOCKET_SENDER
#define SOCKET_SENDER

#include "common.hpp"
#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <boost/asio.hpp>
#include "rawSocketProtocol.hpp"
#include <string>

namespace whisper_library {

/*
	The SocketConnector manages a socket. You can open a connection and send and receive packets. You have to add the
	SocketConnector to the ChannelManager so it gets used.
*/
class SocketSender {

public:
	void setReceiverIp(ulong destinationIpAddress);
	void setReceiverIp(string destinationIpAddress);
	void sendTcp(TcpPacket packet);
	void sendUdp(UdpPacket& packet);


private:
	std::string packetToString(vector<bool> packetData);
	ulong m_ipAddress;
};
}
#endif // SOCKET_SENDER