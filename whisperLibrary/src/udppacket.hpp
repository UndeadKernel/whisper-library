/*	<udppacket.hpp>
	Copyright(C) 2014	Jan Simon Bunten
						Simon Kadel
						Martin Sven Oehler
						Arne Sven Stühlmeyer

	Based on udp_header.hpp (c) 2012 Kevin D. Conley (kcon at stanford dot edu)

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef UDP_PACKET
#define UDP_PACKET

#include <istream>
#include <ostream>
#include <algorithm>
#include <vector>

namespace whisper_library {

class UdpPacket
{
public:
  UdpPacket();

  unsigned short sourcePort() const;
  unsigned short destinationPort() const;
  unsigned short length() const;
  unsigned short checksum() const;
  std::vector<char> data() const;
  std::vector<bool> packet() const;

  void setSourcePort(unsigned short n);
  void setDestinationPort(unsigned short n);
  void setLength(unsigned short n);
  void setChecksum(unsigned short n);
  void setData(std::vector<char> data);
  void setPacket(std::vector<bool> packet);

  //friend std::istream& operator>>(std::istream& is, UdpPacket& header)
  //  { return is.read(reinterpret_cast<char*>(header.rep_), 8); }

  friend std::ostream& operator<<(std::ostream& os, const UdpPacket& packet) {
	  os.write(reinterpret_cast<const char*>(packet.m_head), 8);
	  return os.write(&packet.m_data[0], packet.m_data.size());
  }

private:
  unsigned short decode(int a, int b) const;

  void encode(int a, int b, unsigned short n);

  unsigned char m_head[8];
  std::vector<char> m_data;
};
}
#endif // UDP_PACKET