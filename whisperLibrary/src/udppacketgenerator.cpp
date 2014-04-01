/*	<udppacketgenerator.hpp>
	Copyright(C) 2014   Jan Simon Bunten
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

#include "udppacketgenerator.hpp"
#include <random>
#include <iostream>

namespace whisper_library {

	UdpPacket UdpPacketGenerator::generateNextPacket(unsigned short port) {
		std::random_device generator;
		std::uniform_int_distribution<int> distribution(50, 150);
		unsigned int length = distribution(generator);
		return generatePacketWithLength(port, length);
	}

	UdpPacket UdpPacketGenerator::generatePacketWithLength(unsigned short port, unsigned int length){
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