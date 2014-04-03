#ifndef TCP_PACKET_GENERATOR
#define TCP_PACKET_GENERATOR

#include <functional>
#include "tcppacket.hpp"
#include "genericpacket.hpp"

namespace whisper_library {
	/** \brief Simulates a HTTP conversation based on tcp.

		Creates valid tcp packets by increasing the sequence number and acknowledging incoming packets.
		Packet resending or re-ordering of incoming packets is not implemented.

		Initiate a handshake by calling 'sendConnect'. If a connect request is received, a handshake is initiated as well.
	*/
class TcpPacketGenerator {
public:
	/** \brief Constructor

		Sends a connect request to peer.
		\param port Source and destination port used for creating the packets
		\param send Pointer to a function to send packets over the network
	*/
	TcpPacketGenerator(unsigned short port, function<void(GenericPacket, std::string)> send);
	/** \brief Has to be called when a packet arrives

		Interprets an incoming packet. Responds to connect requests and connect acknowledges.
		Data packets are forwarded to a covert channel using m_forward, afterwards an acknowledge is send.

		\param packet Tcp packet that was received
	*/
	void receivePacket(TcpPacket packet);
	/** \brief Sends a connect request (syn=1).

		Use this at the start of a conversation to initiate a handshake
	*/
	void sendConnect();
	/** \brief Returns a valid tcp packet.

		Creates a tcp data packet using m_next_sequence. The content
		is a http get request if you are sender or an http 404 response if you are server.
		\return valid tcp data packet
	*/
	TcpPacket nextPacket();

	/** \brief Returns the status code of the current connection state
	*/
	unsigned int status();

	static const unsigned int NO_CONNECTION = 0; ///< No connection established
	static const unsigned int RECEIVED_SYN = 1; ///< Connection request received by peer
	static const unsigned int ESTABLISHED = 2; ///< Handshake successful and connection established
private:
	/** \brief Acknowledges a connect request (syn=1, ack=1)
	*/
	void sendConnectResponse();
	/** \brief Responses to a connect request acknowledgement (ack=1)
	*/
	void sendAcknowledgeResponse();
	/** \brief Creates a tcp packet with flags and data
		\param syn Sets the synchronisation flag if true
		\param ack Sets the acknowledgement flag and number if true
		\param data data of the returned packet
		\return valid tcp packet
	*/
	TcpPacket createPacket(bool syn, bool ack, std::string data);

	unsigned short m_port; ///< source/destination port used for communication
	unsigned long m_next_sequence; ///< sequence number used for sending of the next packet
	unsigned long m_base_sequence; ///< highest sequence number that was acknowledge by the receiver
	unsigned long m_next_peer_sequence; ///< next expected sequence number by peer
	function<void(GenericPacket, std::string)> m_send;	///< pointer to a function that is used for sending packets over the network
	unsigned int m_state; ///< stores the internal connection state code (see constants)
	unsigned int m_timeout; ///< timeout in milliseconds used for resending of packets (not used yet)
	bool m_server; ///< true if you are the server after a successful handshake
};

}
#endif // TCP_PACKET_GENERATOR