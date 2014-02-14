//author: Simon Kadel

#include <socketconnector.hpp>

namespace whisper_library{

void SocketConnector::sendTcpPacket(TcpPacket packet) {
	GenericPacket generic_packet(packet.packet());
	m_channelmanager->packetReceived(generic_packet);
}

void SocketConnector::sendUdpPacket(UdpPacket packet) {
	GenericPacket generic_packet(packet.packet());
	m_channelmanager->packetReceived(generic_packet);
}

}

