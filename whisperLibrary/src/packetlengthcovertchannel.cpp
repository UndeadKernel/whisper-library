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
#include <iostream>
#include <thread>

namespace whisper_library {

	const std::string PacketLengthCovertChannel::NAME =
		"Packet Length Covert Channel";
	
	const std::string PacketLengthCovertChannel::INFO =
		"The Packet Length Covert Channel alters the length of UDP packets to transmit messages";

	const std::string PacketLengthCovertChannel::PROTOCOL = "udp";

	const unsigned short PacketLengthCovertChannel::PORT = 23;
	
	PacketLengthCovertChannel::~PacketLengthCovertChannel(){
		delete m_coder;
	}

	void PacketLengthCovertChannel::sendMessage(std::string message){
		std::vector<unsigned int> packetLengths = m_coder->encodeMessage(message);
		m_send(m_get_packet(8 + m_baselength + packetLengths.size()));
		for (unsigned int i = 0; i < packetLengths.size(); i++){
			this_thread::sleep_for(chrono::milliseconds(10));
			m_send(m_get_packet(packetLengths[i]));
		}
	}

	void PacketLengthCovertChannel::receivePacket(GenericPacket& packet){
		UdpPacket udp_packet;
		udp_packet.setPacket(packet.packet());
		unsigned int length = udp_packet.length();
		if (m_packetcount == -1){
			if (length == 8 + m_baselength){
				m_output("");
				return;
			}
			m_packetcount = length - 8 - m_baselength;
		}
		else {
			m_packetlengths.push_back(length);
			m_received++;
		}
		if (m_received == m_packetcount){
			m_output(m_coder->decodeMessage(m_packetlengths));
			m_packetlengths.clear();
			m_received = 0;
			m_packetcount = -1;
		}
	}

	std::string PacketLengthCovertChannel::name() const {
		return NAME;
	}

	std::string PacketLengthCovertChannel::info() const {
		return INFO;
	}

	std::string PacketLengthCovertChannel::protocol() const {
		return PROTOCOL;
	}
	
	unsigned short PacketLengthCovertChannel::port() const {
		return PORT;
	}
}