/*	<udppacket.cpp>
	Copyright(C) 2014   Jan Simon Bunten
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
#include "udppacket.hpp"
#include <bitset>


namespace whisper_library {

UdpPacket::UdpPacket(){
	std::fill(m_head, m_head + sizeof(m_head), 0);
}

UdpPacket::UdpPacket(std::vector<bool> content){
	setPacket(content);
}

unsigned short UdpPacket::sourcePort() const { 
	return decode(0, 1); 
}

unsigned short UdpPacket::destinationPort() const {
	return decode(2, 3);
}

unsigned short UdpPacket::length() const {
	return decode(4, 5);
}

unsigned short UdpPacket::checksum() const {
	return decode(6, 7);
}

std::vector<char> UdpPacket::data() const {
	return m_data;
}

std::vector<bool> UdpPacket::packet() const {
	std::vector<bool> result;
	for (unsigned int i = 0; i < 8; i++){
		std::bitset<8> bits(m_head[i]);
		for (int j = 0; j < 8; j++){
			result.push_back(bits[j]);
		}
	}
	for (unsigned int i = 0; i < m_data.size(); i++){
		std::bitset<8> bits(m_data[i]);
		for (int j = 0; j < 8; j++){
			result.push_back(bits[j]);
		}
	}
	return result;
}

void UdpPacket::setSourcePort(unsigned short port) {
	encode(0, 1, port);
}

void UdpPacket::setDestinationPort(unsigned short port) {
	encode(2, 3, port);
}

void UdpPacket::setLength(unsigned short length) {
	encode(4, 5, length);
}

void UdpPacket::setChecksum(unsigned short checksum) {
	encode(6, 7, checksum);
}

void UdpPacket::setData(std::vector<char> data) {
	m_data = data;
}

void UdpPacket::setPacket(std::vector<bool> packet){
	std::vector<char> parts;
	for (unsigned int i = 0; i < packet.size() / 8; i++){
		std::bitset<8> part;
		for (int j = 0; j < 8; j++){
			part[j] = packet[i * 8 + j];
		}
		parts.push_back(part.to_ulong());
	}
	for (unsigned int j = 0; j < 8; j++){
		m_head[j] = parts[j];
	}
	m_data = std::vector<char>(parts.begin() + 8, parts.end());
}


unsigned short UdpPacket::decode(int start, int end) const {
	return (m_head[start] << 8) + m_head[end];
}

void UdpPacket::encode(int start, int end, unsigned short value) {
    m_head[start] = static_cast<unsigned char>(value >> 8);
    m_head[end] = static_cast<unsigned char>(value & 0xFF);
}
}