#include "socketSender.hpp"
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace boost::asio;

namespace whisper_library {

void SocketSender::setReceiverIp(ulong destinationIpAddress){
	m_ipAddress = destinationIpAddress;	
}

void SocketSender::setReceiverIp(string destinationIpAddress) {
	vector<string> parts;
	boost::split(parts, destinationIpAddress, boost::is_any_of("."), boost::token_compress_on);
	ulong ip = 0;
	try {
		ip += boost::lexical_cast<int>(parts[3]);
		ip += boost::lexical_cast<int>(parts[2]) << 8;
		ip += boost::lexical_cast<int>(parts[1]) << 16;
		ip += boost::lexical_cast<int>(parts[0]) << 24;
	}
	catch (boost::bad_lexical_cast e) {
		cout << "Error: " << e.what() << endl;
	}

	m_ipAddress = ip;
}

void SocketSender::sendTcp(TcpPacket packet){
	//TODO eigene IP bestimmen
	ulong ownIp = 3232236136;
	packet.calculateChecksum(ownIp, m_ipAddress, 0, 6);
	io_service io_service;
	basic_raw_socket<ip::RawSocketProtocol<IPPROTO_TCP>> socket(io_service);
	
	//TODO irgendwie die Daten in den send_buffer packen
	vector<bool> packetData = packet.packet();
	std::string packetString = packetToString(packetData);
	boost::asio::streambuf send_buffer;
    std::ostream os(&send_buffer);
	for (int i = 0; i < packetString.size(); i++){
		os.put(packetString[i]);
	}
	ip::RawSocketProtocol<IPPROTO_TCP>::endpoint ep(ip::address_v4(m_ipAddress), packet.destPort());
    try {
        socket.open();
		socket.send_to(send_buffer.data(), ep);
		socket.close();
    } catch (std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
    }
}

void SocketSender::sendUdp(UdpPacket packet) {
		io_service io_service;
		basic_raw_socket<ip::RawSocketProtocol<IPPROTO_UDP> > socket(io_service);
		boost::asio::streambuf request_buffer;
		std::ostream os(&request_buffer);
		os << packet;
		ip::RawSocketProtocol<IPPROTO_UDP>::endpoint ep(ip::address_v4(m_ipAddress), packet.destinationPort());
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
