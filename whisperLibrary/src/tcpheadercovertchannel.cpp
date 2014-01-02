// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>

using namespace std;

namespace whisper_library {

	whisper_library::TcpPacket TcpHeaderCovertChannel::modifyTcpPacket(whisper_library::TcpPacket packet) {

	}

	void TcpHeaderCovertChannel::setMessage(string message) {
		m_message = message;
	}

}