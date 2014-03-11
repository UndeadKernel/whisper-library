/*	<packetlengthcovertchannel.cpp>
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

#include <packetlengthcovertchannel.hpp>

namespace whisper_library {
	

	PacketLengthCovertChannel::~PacketLengthCovertChannel(){
		delete m_coder;
	}

	void PacketLengthCovertChannel::sendMessage(std::string message){
		m_packetLengths = m_coder->encodeMessage(message);
		m_send(m_getPacket(8 + m_baseLength + m_packetLengths.size()));
		for (int i = 0; i < m_packetLengths.size(); i++){
			m_send(m_getPacket(m_packetLengths[i]));
		}

	}

	void PacketLengthCovertChannel::receivePacket(GenericPacket& packet){
		UdpPacket udpPacket;
		udpPacket.packet() = packet.content();
		if (m_packetCount == -1){
			m_packetCount = udpPacket.length()-8-m_baseLength;
		}
		else {
			m_packetLengths.push_back(udpPacket.length());
		}
		m_received++;
		if (m_received == m_packetCount){
			m_output(m_coder->decodeMessage(m_packetLengths));
			m_received = 0;
			m_packetCount = -1;
		}

	}

	std::string PacketLengthCovertChannel::name() const {
		return "Packet Length Covert Channel";
	}

	std::string PacketLengthCovertChannel::info() const {
		return "The Packet Length Covert Channel alters the length of UDP packets to transmit messages";
	}

}