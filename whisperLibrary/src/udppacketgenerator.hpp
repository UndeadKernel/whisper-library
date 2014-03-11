#ifndef UDP_PACKET_GENERATOR
#define UDP_PACKET_GENERATOR

#include "udppacket.hpp"

namespace whisper_library {
/*
	This class provides a function to create valid udp packets with random content and length.
*/
class UdpPacketGenerator {
public:
	/*
		'nextPacket' returns a valid udp packet  with random content and length.
		Call the function with the port, the packet is sent to. The length of the packet is random (not pseudo)
		and uniformly distributed between 50 and 150 bytes. It contains uniformly distributed random characters.
	*/
	static UdpPacket nextPacket(unsigned short port);
	static UdpPacket packetWithLength(unsigned short port, unsigned int length);
};
}
#endif // UDP_PACKET_GENERATOR