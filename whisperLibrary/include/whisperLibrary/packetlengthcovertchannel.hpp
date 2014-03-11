/*	<packetlengthcovertchannel.hpp>
	Copyright(C) 2014	Jan Simon Bunten
						Simon Kadel
						Martin Sven Oehler
						Arne Sven Stühlmeyer

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

#ifndef PACKET_LENGTH_COVERT_CHANNEL
#define PACKET_LENGTH_COVERT_CHANNEL

#include "covertchannel.hpp"
#include <functional>
#include "../../src/udppacket.hpp"
#include "lengthcoder.hpp"

namespace whisper_library {

class PacketLengthCovertChannel : public CovertChannel {
public:
	/** \brief Creates an PacketLenghtCovertChannel

		Creates a new PacketLengthCovertChannel. An PacketLengthCovertChannel needs three function pointers.
		It than can be use to send and receive messages.
		\param output a function the covert channel calls with the message he received as an argument
		\param send a function the covert channel uses to send udp packets
		\param getPacket a function the covert channel uses to get udp packets with a certain length
		
	*/
	PacketLengthCovertChannel(function<void(std::string)> output,
		function<void(UdpPacket)> send,
		function<UdpPacket(int)> getPacket)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_getPacket(getPacket),
		m_received(0),
		m_packetCount(-1),
		m_baseLength(10),
		m_coder(new LengthCoder(m_baseLength))
	{};
	/** \brief Deletes the PacketLengthCovertChannel

		Deletes this instance of PacketLengthCovertChannel and frees all memory.
	*/
	~PacketLengthCovertChannel();
	/** \brief sends a message over the packet length covert channel

		The message gets encoded as packet lengths and then udp packets with this lengths get
		send using the function passed in the constructor.
		\param message the message that is send
	*/
	void sendMessage(std::string message);
	/** \brief interprets an udp packet
		
		The given packet is interpreted as an udp packet and the length of the packet is used to
		reassemble the message.
		\param udpPacket the packet that gets interpreted
	*/
	void receivePacket(GenericPacket& udpPacket);

	std::string name() const;
	std::string info() const;
private:
	
	function<void(string)> m_output;///< function used to return received messages as a string
	function<void(UdpPacket)> m_send;///< function used to send Udp Packets via the socket
	function<UdpPacket(int)> m_getPacket;///< function used to retrieve valid udp packets with given length
	std::vector<unsigned int> m_packetLengths;///< holds the packet lengths send or received
	int m_received;///< counts the received packets
	int m_packetCount;///< number of packets for the current transmission
	int m_baseLength;///< minimal length of data payload of the udp packets
	LengthCoder* m_coder;///< used to encode and decode messages
};
}
#endif // PACKET_LENGTH_COVERT_CHANNEL