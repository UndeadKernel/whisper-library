#include "udppacketgenerator.hpp"
#include <random>
#include <iostream>

namespace whisper_library {

	UdpPacket UdpPacketGenerator::nextPacket(unsigned short port) {
		std::random_device generator;
		std::uniform_int_distribution<int> distribution(50, 150);
		unsigned int length = distribution(generator);
		return packetWithLength(port, length);
	}

	UdpPacket UdpPacketGenerator::packetWithLength(unsigned short port, unsigned int length){
		whisper_library::UdpPacket packet;
		// Header
		packet.setSourcePort(port);
		packet.setDestinationPort(port);
		packet.setChecksum(0);
		// Data
		std::vector<char> data;
		std::random_device generator;
		std::uniform_int_distribution<int> char_distribution(0, 255);
		for (unsigned int i = 0; i < length-8; i++) {
			unsigned int character = char_distribution(generator);
			data.push_back(static_cast<char>(character));
		}
		packet.setData(data);
		packet.setLength(8 + data.size());
		return packet;
	}
}