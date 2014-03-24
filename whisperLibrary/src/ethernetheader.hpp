#ifndef ETHERNET_HEADER
#define ETHERNET_HEADER

#include <string>
#include <vector>

using namespace std;
namespace whisper_library {
class EthernetHeader {
public:
	EthernetHeader();
	EthernetHeader(unsigned char* header);
	~EthernetHeader();
	string sourceMAC();
	string destinationMAC();
	unsigned long ethernetType();

	void setSourceMAC(string mac);
	void setSourceMAC(unsigned char* mac_bitstring);
	void setDestinationMAC(string mac);
	void setDestinationMAC(unsigned char* mac_bitstring);
	void setEthernetType(unsigned long type);

	static string toMacString(unsigned char* buffer);
	vector<bool> toVector();
	string toString();
private:
	static string intToHex(unsigned int i);
	unsigned char hexToInt(string hex);
	unsigned char * m_head;
};

}
#endif // ETHERNET_HEADER