#ifndef UDP_PACKET_GENERATOR
#define UDP_PACKET_GENERATOR

#include "udppacket.hpp"

namespace whisper_library {
/** \brief generates valid udp packets

	This class provides a function to create valid udp packets with random content and length.
*/
class UdpPacketGenerator {
public:
	/** \brief generates a udp packet

		'nextPacket' returns a valid udp packet  with random content and length.
		Call the function with the port, the packet is sent to. The length of the packet is random
		and uniformly distributed between 50 and 150 bytes. It contains uniformly distributed random characters.
	*/
	static UdpPacket nextPacket(unsigned short port);
	/** \brief generates a udp packet with specific length

		Generates a UdpPacket with the given length by filling the packet with random characters.
		If the given length is less than eight, a udp packet without data is returned.
		\param port the source and destination port
		\param length the desired length of the packet
		\returns a valid udp packet with the desired length
	*/
	static UdpPacket packetWithLength(unsigned short port, unsigned int length);
};
}
#endif // UDP_PACKET_GENERATOR