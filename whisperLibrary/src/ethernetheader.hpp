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
	string sourceMAC();
	string destinationMAC();
	unsigned long ethernetType();

	void setSourceMAC(string mac);
	void setDestinationMAC(string mac);
	void setEthernetType(unsigned long type);

	vector<bool> toVector();
	string toString();
private:
	string intToHex(unsigned int i);
	unsigned char hexToInt(string hex);
	unsigned char * m_head;
};

}
#endif // ETHERNET_HEADER