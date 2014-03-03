#include "socketSender.hpp"
#include <boost/array.hpp>
#include <boost/algorithm/string.hpp>

using namespace boost::asio;

namespace whisper_library {

void SocketSender::setReceiverIp(ulong destinationIpAddress){
	m_ipAddress = destinationIpAddress;	
}

void SocketSender::setReceiverIp(string destinationIpAddress) {
	vector<string> parts;
	boost::split(parts, destinationIpAddress, boost::is_any_of("."), boost::token_compress_on);
	ulong ip = 0;
	ip += atoi(parts[3].c_str());
	ip += atoi(parts[2].c_str()) << 8;
	ip += atoi(parts[1].c_str()) << 16;
	ip += atoi(parts[0].c_str()) << 24;
	
	m_ipAddress = ip;
}

void SocketSender::sendTcp(TcpPacket packet){
	//TODO eigene IP bestimmen
	//ulong ownIp = 127 << 24;
	//ownIp += 1;
	ulong ownIp = 3232236137;
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
	ip::RawSocketProtocol<IPPROTO_TCP>::endpoint ep(ip::address_v4(m_ipAddress), 8080);
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