// Author: Martin Oehler

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include <tcppacket.hpp>
#include <bitSetEncoder.hpp>
#include <string>

using namespace std;

namespace whisper_library {

class WHISPERAPI TcpHeaderCovertChannel {

public:
	void setMessage(string message);

private:
	whisper_library::TcpPacket modifyTcpPacket(whisper_library::TcpPacket packet);
	string m_message = "";
};

}

#endif // TCP_HEADER_COVERT_CHANNEL