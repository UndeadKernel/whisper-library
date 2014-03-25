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

	static const unsigned int NO_CONNECTION = 0;
	static const unsigned int RECEIVED_SYN = 1;
	static const unsigned int ESTABLISHED = 2;
private:
	void sendConnect();
	void sendConnectResponse();
	void sendAcknowledgeResponse();

	unsigned short m_port;
	unsigned long m_send_sequence; ///< sequence number used for sending
	unsigned long m_base_sequence; ///< sequence numbers that were acknowledge by the receiver
	unsigned long m_peer_sequence; ///< sequence number received by peer
	function<void(TcpPacket)> m_send;
	function<void(GenericPacket)> m_forward;
	unsigned int m_state;
	unsigned int m_timeout;
};

}
#endif // TCP_PACKET_GENERATOR