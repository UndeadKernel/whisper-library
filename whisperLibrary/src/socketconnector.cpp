//author: Simon Kadel

#include <socketconnector.hpp>

namespace whisper_library{

void SocketConnector::sendPacket(TcpPacket packet) {
	m_channelmanager->packetReceived(packet);
}

}

