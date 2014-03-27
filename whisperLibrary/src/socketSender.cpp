/*	<socketSender.cpp>
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

#include "socketSender.hpp"
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include "rawSocketProtocol.hpp"
#include <boost/asio.hpp>

using namespace boost::asio;

namespace whisper_library {

void SocketSender::sendTcp(string source_ip, string destination_ip, TcpPacket packet){
	//calculate checksum
	ulong ulong_source_ip = ipToUlong(source_ip);
	ulong ulong_destination_ip = ipToUlong(destination_ip);
	packet.calculateChecksum(ulong_source_ip, ulong_destination_ip, 0, 6);
	//set up socket
	io_service io_service;
	basic_raw_socket<ip::RawSocketProtocol<IPPROTO_TCP>> socket(io_service);
	ip::RawSocketProtocol<IPPROTO_TCP>::endpoint ep(ip::address_v4(ulong_destination_ip),
													packet.destPort());	
	//prepare data
	vector<bool> packetData = packet.packet();
	std::string packetString = packetToString(packetData);
	boost::asio::streambuf send_buffer;
    std::ostream os(&send_buffer);
	for (int i = 0; i < packetString.size(); i++){
		os.put(packetString[i]);
	}
	//send packet
    try {
        socket.open();
		socket.send_to(send_buffer.data(), ep);
		socket.close();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void SocketSender::sendUdp(string destination_ip, UdpPacket packet) {
	//set up socket
	ulong ulong_destination_ip = ipToUlong(destination_ip);
	io_service io_service;
	basic_raw_socket<ip::RawSocketProtocol<IPPROTO_UDP> > socket(io_service);
	ip::RawSocketProtocol<IPPROTO_UDP>::endpoint ep(ip::address_v4(ulong_destination_ip),
													packet.destinationPort());
	//prepare data
	boost::asio::streambuf request_buffer;
	std::ostream os(&request_buffer);
	os << packet;
	//send packet
	try {
		socket.open();
		socket.send_to(request_buffer.data(), ep);
		socket.close();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}

std::string SocketSender::packetToString(vector<bool> packet_data){
	std::string result;
	for (int i = 0; i < packet_data.size()/8; i++){
		int ascii = 0;
		int value = 1;
		for (int j = 7; j >= 0; j--) {
			if (packet_data[i*8 + j]) {
				ascii += value;
			}
			value = value << 1;
		}
		char c = (char) ascii;
		result.push_back(c);
	}
	return result;
}

ulong SocketSender::ipToUlong(string ip) {
	vector<string> parts;
	boost::split(parts, ip, boost::is_any_of("."), boost::token_compress_on);
	ulong ulong_ip = 0;
	try {
		ulong_ip += boost::lexical_cast<unsigned int>(parts[3]);
		ulong_ip += boost::lexical_cast<unsigned int>(parts[2]) << 8;
		ulong_ip += boost::lexical_cast<unsigned int>(parts[1]) << 16;
		ulong_ip += boost::lexical_cast<unsigned int>(parts[0]) << 24;
	}
	catch (boost::bad_lexical_cast e) {
		cout << "Error: " << e.what() << endl;
	}
	return ulong_ip;
}
}
