/*	<socketconnector.cpp>
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

#include <socketconnector.hpp>

namespace whisper_library{

void SocketConnector::sendTcpPacket(TcpPacket packet) {
	//GenericPacket generic_packet(packet.packet());
	//m_channelmanager->packetReceived(generic_packet);
}

void SocketConnector::sendUdpPacket(UdpPacket packet) {
	//GenericPacket generic_packet(packet.packet());
	//m_channelmanager->packetReceived(generic_packet);
}

}

