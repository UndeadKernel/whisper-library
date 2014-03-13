#ifndef IP_PACKET_V4
#define IP_PACKET_V4

#include <vector>
#include <string>

using namespace std;

namespace whisper_library {
class IpHeaderv4 {
public:
	IpHeaderv4();
	// frame in little endian
	IpHeaderv4(vector<bool> frame);

	// IP-Version (4 or 6) (4 bit)
	unsigned int version();
	// length of the IP header : length * 32 bit (4 bit)
	unsigned int ipHeaderLength();
	// used for packet prioritization (8 bit)
	unsigned int typeOfService();
	// total length of the packet in byte(maximum 65536 Byte = 64 KiB) (16 bit)
	unsigned int totalLength();
	// unique identifier to reassemble fragmented IP-Datagrams (16 bit)
	unsigned int identification();
	// reserved, has to be 0 (1 bit)
	bool flagReserved();
	// Don't fragment (1 bit)
	bool flagDontFragment();
	// More Fragments (0 if only or last fragment) (1 bit)
	bool flagFragmented();
	// fragment position (0 if not fragmented) (12 bit)
	unsigned int offset();
	// time to live, drop packet if 0, reduce at each hop (4 bit)
	unsigned int timeToLive();
	// application layer protocol, TCP -> 6, UDP -> 17 (4 bit)
	unsigned int protocol();
	// Ip header checksum (16 bit)
	unsigned int checksum();
	// Ip address of the sender (32 bit)
	unsigned long sourceIp();
	string sourceIpDotted();
	// Ip address of the receiver (32 bit)
	unsigned long destinationIp();
	string destinationIpDotted();
	// print header content to stdout
	void print();
	string info();

	static const unsigned int TCP = 6;
	static const unsigned int UDP = 17;

private:
	string ipToDotted(unsigned long ip);
	unsigned char* m_head;
};
}

#endif