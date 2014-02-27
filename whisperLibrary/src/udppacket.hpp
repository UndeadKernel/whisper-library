#ifndef UDP_PACKET
#define UDP_PACKET

#include <istream>
#include <ostream>
#include <algorithm>
#include <vector>

namespace whisper_library {

// UDP header for both IPv4 and IPv6.
//
// The wire format of a UDP header is:
//
// 0 8 16 31
// +-------------------------------+------------------------------+ ---
// | | | ^
// | source port | destination port | |
// | | | |
// +-------------------------------+------------------------------+ 8 bytes
// | | | |
// | length | checksum | |
// | | | v
// +-------------------------------+------------------------------+ ---
//
// The checksum field is optional for IPv4 but is required by IPv6.

class UdpPacket
{
public:
  UdpPacket();

  unsigned short sourcePort() const;
  unsigned short destinationPort() const;
  unsigned short length() const;
  unsigned short checksum() const;
  std::vector<char> data() const;

  void setSourcePort(unsigned short n);
  void setDestinationPort(unsigned short n);
  void setLength(unsigned short n);
  void setChecksum(unsigned short n);
  void setData(std::vector<char> data);

  //friend std::istream& operator>>(std::istream& is, UdpPacket& header)
  //  { return is.read(reinterpret_cast<char*>(header.rep_), 8); }

  friend std::ostream& operator<<(std::ostream& os, const UdpPacket& packet) {
	  os.write(reinterpret_cast<const char*>(packet.m_head), 8);
	  return os.write(&packet.m_data[0], packet.m_data.size());
  }

  void to_string() {
	  unsigned int i;
	  printf("{");
	  for (i = 0; i < sizeof(m_head); i++) {
		  printf("%.2X, ", m_head[i]);
	  }
	  for (i = 0; i < m_data.size(); i++) {
		  printf(i == m_data.size() - 1 ? "%.2X}\n" : "%.2X, ", m_data[i]);
	  }
  }

private:
  unsigned short decode(int a, int b) const;

  void encode(int a, int b, unsigned short n);

  unsigned char m_head[8];
  std::vector<char> m_data;
};
}
#endif // UDP_PACKET