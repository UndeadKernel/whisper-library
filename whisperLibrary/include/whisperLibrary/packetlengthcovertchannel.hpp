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
	/** \brief A covert channel using packet lengths

		A PacketLengthCovertChannel can be used to send and receive messages coded in the
		length of udp packets. It uses a base length of ten and increases this length
		depending on the send message.
	*/
class PacketLengthCovertChannel : public CovertChannel {
public:
	/** \brief Creates an PacketLengthCovertChannel

		Creates a new PacketLengthCovertChannel. An PacketLengthCovertChannel needs three
		function pointers. It than can be use to send and receive messages.
		\param output a function the covert channel calls with the message he received as 
					an argument
		\param send a function the covert channel uses to send udp packets		
	*/
	PacketLengthCovertChannel(function<void(std::string)> output,
		function<void(GenericPacket, std::string)> send)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_received(0),
		m_packetcount(-1),
		m_baselength(10),
		m_coder(new LengthCoder(m_baselength)){};

	PacketLengthCovertChannel()
		: CovertChannel(),
		m_received(0),
		m_packetcount(-1),
		m_baselength(10),
		m_coder(new LengthCoder(m_baselength)){};

	/** \brief Deletes the PacketLengthCovertChannel

		Deletes this instance of PacketLengthCovertChannel and frees all memory.
	*/
	~PacketLengthCovertChannel();

	CovertChannel* instance();
	/** \brief sends a message

		The message gets encoded as packet lengths and then udp packets with this lengths get
		send using the function passed in the constructor.
		\param message the message that is send
	*/
	void sendMessage(std::string message);
	/** \brief interprets an udp packet
		
		The given packet is interpreted as an udp packet and the length of the packet is used to
		reassemble the message.
		\param udp_packet the packet that gets interpreted
	*/
	void receivePacket(GenericPacket& udp_packet);
	/**
		No arguments, empty function
	*/
	void setArguments(std::string arguments) {};
	/** \brief sets the function used to output received messages

		\param output a function-pointer used to output messages
	*/
	void setOutput(function<void(std::string)> output);
	/** \brief sets the function used to send packets

		\param send a function-pointer used to send packets
	*/
	void setSend(function<void(GenericPacket, std::string)> send);
	/** \return The name of the covert channel
	*/
	std::string name() const;
	/** \return Some information about the covert channel
	*/
	std::string info() const;
	/** \return Protocol used by this covert channel
	*/
	std::string protocol() const;
	/** \return Port that packets are send to
	*/
    unsigned short port() const;
	/** \return the ID of the covert channel
	*/
	std::string id() const;

private:
	std::function<void(std::string)> m_output;///< function used to return received messages as a string
	std::function<void(GenericPacket, std::string)> m_send;///< function used to send Udp Packets via the socket
	std::vector<unsigned int> m_packetlengths;///< holds the packet lengths send or received
	int m_received;///< counts the received packets
	int m_packetcount;///< number of packets for the current transmission
	int m_baselength;///< minimal length of data payload of the udp packets
	LengthCoder* m_coder;///< used to encode and decode messages
};
}
#endif // PACKET_LENGTH_COVERT_CHANNEL