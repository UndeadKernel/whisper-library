/*	<socketSender.hpp>
	Copyright(C) 2013, 2014	Jan Simon Bunten
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

#ifndef SOCKET_SENDER
#define SOCKET_SENDER

#include "tcppacket.hpp"
#include "udppacket.hpp"
#include <string>

namespace whisper_library {

/** \brief Sends packets over a raw Socket

	This class can be used to send packets to a destination ip address. It uses
	raw sockets to do so. On unix tcp and udp packets can be send, on windows, since
	XP SP3,	the use of raw sockets is restricted, only udp packets can be send.
*/
class SocketSender {

public:
	/** \brief sends a tcp packet to destination ip address

		The checksum for the packet is calculated based on the given ip addresses
		and than the packet is send over a raw socket.
		\param source_ip the senders ip address
		\param destination_ip the receivers ip address
		\param packet the packet that gets send
	*/
	void sendTcp(string source_ip, string destination_ip, TcpPacket packet);
	/** \brief sends a udp packet to destination ip address

		\param destination_ip the receivers ip address
		\param packet the packet that gets send
	*/
	void sendUdp(string destination_ip, UdpPacket packet);

private:
	/** \brief converts packet data to string

		The packet data gets converted from vector<bool> to std::string, so it
		can be processed easily.
		\param packetData the data to convert
		\return a text representation of the data
	*/
	std::string packetToString(vector<bool> packetData);
	/** \brief converts a ip from string to ulong representation

		\param ip the IPv4 as text
		\returns the IPv4 as ulong
	*/
	ulong ipToUlong(string ip);
};
}
#endif // SOCKET_SENDER