/*	<packetlengthcovertchannel.hpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
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
#include "udppacket.hpp"

namespace whisper_library {

class PacketLengthCovertChannel : public CovertChannel {
public:
	PacketLengthCovertChannel(function<void(std::string)> output,
		function<void(UdpPacket)> send,
		function<UdpPacket(int)> getPacket)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_getPacket(getPacket),
		m_received(0),
		m_packetCount(-1),
		m_baseLength(10)
	{};
	~PacketLengthCovertChannel();
	void sendMessage(std::string message);
	void receiveMessage(GenericPacket& packet);
	std::string name() const;
	std::string info() const;
private:
	// callback function pointer that is used to return received messages as a string
	function<void(string)> m_output;

	// function pointer that is used to send Udp Packets via the socket
	function<void(UdpPacket)> m_send;

	// function pointer that is used to retrieve valid udp packets, that are send with delay
	function<UdpPacket(int)> m_getPacket;

	std::vector<int> m_packetLengths;

	int m_received;
	int m_packetCount;
	int m_baseLength;
};
}
#endif // PACKET_LENGTH_COVERT_CHANNEL