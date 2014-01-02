// Author: Martin Oehler

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include <tcppacket.hpp>
#include <bitSetEncoder.hpp>
#include <bitSetDecoder.hpp>
#include <string>
#include <vector>
#include <bitset>

using namespace std;

namespace whisper_library {

class WHISPERAPI TcpHeaderCovertChannel {

public:
	void addMessage(string message);

private:
	void modifyTcpPacket(whisper_library::TcpPacket& packet);
	void receiveMessage(whisper_library::TcpPacket& packet);

	bitset<6> getNextMessageBlock();

	//TODO umstellen auf deque (queue)
	vector<bitset<6>> m_message_blocks;
	vector<bitset<6>> m_received_message_blocks;
	string m_output_string;
};

}

#endif // TCP_HEADER_COVERT_CHANNEL