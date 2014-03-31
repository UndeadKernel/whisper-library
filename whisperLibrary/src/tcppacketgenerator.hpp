#ifndef TCP_PACKET_GENERATOR
#define TCP_PACKET_GENERATOR

#include <functional>
#include "tcppacket.hpp"
#include "genericpacket.hpp"

namespace whisper_library {
class TcpPacketGenerator {
public:
	TcpPacketGenerator(unsigned short port, function<void(TcpPacket)> send, function<void(GenericPacket)> forward);
	void receivePacket(TcpPacket);
	TcpPacket nextPacket();

	unsigned int status();

	static const unsigned int NO_CONNECTION = 0;
	static const unsigned int RECEIVED_SYN = 1;
	static const unsigned int ESTABLISHED = 2;
private:
	void sendConnect();
	TcpPacket createPacket(bool syn, bool ack, string data);
	void sendConnectResponse();
	void sendAcknowledgeResponse();

	unsigned short m_port;
	unsigned long m_next_sequence; ///< sequence number used for sending of the next packet
	unsigned long m_base_sequence; ///< highest sequence number that was acknowledge by the receiver
	unsigned long m_next_peer_sequence; ///< next expected sequence number by peer
	function<void(TcpPacket)> m_send;
	function<void(GenericPacket)> m_forward;
	unsigned int m_state;
	unsigned int m_timeout;
	bool m_server;
};

}
#endif // TCP_PACKET_GENERATOR