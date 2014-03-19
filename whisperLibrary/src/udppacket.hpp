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
#include "genericpacket.hpp"

namespace whisper_library {
/** \brief Represents a udp packet

	A UdpPacket represents a udp packet, that means the udp header an the
	data payload. It can be used to store and access all fields and it offers
	a stream operator to write it in  std::ostream.
*/
class UdpPacket : public GenericPacket {
public:
	/** \brief Creates an empty UdpPacket.	
	*/
	UdpPacket();
	/** \brief Creates an UdpPacket with given content
		\param content the bits that are used to fill the packet
	*/
	UdpPacket(std::vector<bool> content);
	/**
		\return the source port (byte 1 and 2)
	*/
	unsigned short sourcePort() const;
	/**
		\return the destination port (byte 3 and 4)
	*/
	unsigned short destinationPort() const;
	/**
		\return the packet length (byte 5 and 6)
	*/
	unsigned short length() const;
	/**
		\return the checksum (byte 7 and 8)
	*/
	unsigned short checksum() const;
	/**
		\return the data payload
	*/
	std::vector<char> data() const;
	/**
		\return the complete packet as std::vector<bool>
	*/
	std::vector<bool> packet() const;
	/**
		sets the source port (byte 1 and 2)
		\param value the new source port
	*/
	void setSourcePort(unsigned short value);
	/**
		sets the destination port (byte 3 and 4)
		\param value the new destination port
	*/
	void setDestinationPort(unsigned short value);
	/**
		sets the packet length (byte 5 and 6)
		\param value the new packet length
	*/
	void setLength(unsigned short value);
	/**
		sets the checksum (byte 7 and 8)
		\param value the new checksum
	*/
	void setChecksum(unsigned short value);
	/**
		sets the data payload
		\param data the new data payload
	*/
	void setData(std::vector<char> data);
	/**
		sets the whole packet
		\param packet the new packet as bits
	*/
	void setPacket(std::vector<bool> packet);
	/**
		streamoperator to write the whole packet to a std::ostream
	*/
	friend std::ostream& operator<<(std::ostream& ostream, const UdpPacket& packet) {
		ostream.write(reinterpret_cast<const char*>(packet.m_head), 8);
		return ostream.write(&packet.m_data[0], packet.m_data.size());
	}

private:
	/** \brief returns the headerfield between start and end
	
		The bytes of the m_head between start and end get interpreted as unsigned short
		and returned. Because unsigned short only holds two byte, start and end should
		be succesive.
		\param start the index of the first byte
		\param end the index of the last byte
		\returns the value beetween start and end
	*/
	unsigned short decode(int start, int end) const;
	/** \brief writes the given value in the headerfield beetween start and end
	
		The two bytes of value are written in m_head at the indices start and end. Start
		and end should be succesive.
		\param start the index for the first half of value
		\param end the index for the seconde halt of value
		\param value the two bytes that are written in m_head
	*/
	void encode(int start, int end, unsigned short value);

	unsigned char m_head[8];///< Holds the header as unsigned characters
	std::vector<char> m_data;///< Holds the data as characters
};
}
#endif // UDP_PACKET