#include "socketSender.hpp"
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>

using namespace boost::asio;

namespace whisper_library {

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

void SocketSender::sendTcp(string source_ip, string destination_ip, TcpPacket packet){
	ulong ulong_source_ip = ipToUlong(source_ip);
	ulong ulong_destination_ip = ipToUlong(destination_ip);

	packet.calculateChecksum(ulong_source_ip, ulong_destination_ip, 0, 6);

	io_service io_service;
	basic_raw_socket<ip::RawSocketProtocol<IPPROTO_TCP>> socket(io_service);
	
	vector<bool> packetData = packet.packet();
	std::string packetString = packetToString(packetData);
	boost::asio::streambuf send_buffer;
    std::ostream os(&send_buffer);
	for (int i = 0; i < packetString.size(); i++){
		os.put(packetString[i]);
	}
	ip::RawSocketProtocol<IPPROTO_TCP>::endpoint ep(ip::address_v4(ulong_destination_ip), packet.destPort());
    try {
        socket.open();
		socket.send_to(send_buffer.data(), ep);
		socket.close();
    } catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

void SocketSender::sendUdp(string destination_ip, UdpPacket packet) {
	ulong ulong_destination_ip = ipToUlong(destination_ip);
	io_service io_service;
	basic_raw_socket<ip::RawSocketProtocol<IPPROTO_UDP> > socket(io_service);
	boost::asio::streambuf request_buffer;
	std::ostream os(&request_buffer);
	os << packet;
	ip::RawSocketProtocol<IPPROTO_UDP>::endpoint ep(ip::address_v4(ulong_destination_ip), packet.destinationPort());
	unsigned short checksum = packet.checksum();
	try {
		socket.open();
		socket.send_to(request_buffer.data(), ep);
		socket.close();
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}


std::string SocketSender::packetToString(vector<bool> packetData){
	std::string result;
	for (int i = 0; i < packetData.size()/8; i++){
		int ascii = 0;
		int value = 1;
		for (int j = 7; j >= 0; j--) {
			if (packetData[i*8 + j]) {
				ascii += value;
			}
			value = value << 1;
		}
		char c = (char) ascii;
		result.push_back(c);
	}
	return result;
}
}
