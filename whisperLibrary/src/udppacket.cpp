#include "udppacket.hpp"


namespace whisper_library {

UdpPacket::UdpPacket(){
	std::fill(m_head, m_head + sizeof(m_head), 0);
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


unsigned short UdpPacket::decode(int start, int end) const {
	return (m_head[start] << 8) + m_head[end];
}

void UdpPacket::encode(int start, int end, unsigned short value) {
    m_head[start] = static_cast<unsigned char>(value >> 8);
    m_head[end] = static_cast<unsigned char>(value & 0xFF);
}
}