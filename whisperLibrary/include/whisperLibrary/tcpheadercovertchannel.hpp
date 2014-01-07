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
#include <channelmanager.hpp>

using namespace std;

namespace whisper_library {

class WHISPERAPI TcpHeaderCovertChannel {

public:
	TcpHeaderCovertChannel(whisper_library::ChannelManager* channelmanager);
	vector<whisper_library::TcpPacket> sendMessage(string message);
	void receiveMessage(whisper_library::TcpPacket& packet);

private:
	void modifyTcpPacket(whisper_library::TcpPacket& packet, bitset<6> data);
	bitset<6> extractData(whisper_library::TcpPacket& packet);
	vector<bitset<6>> m_data_blocks;
	int m_numb_packets;
	whisper_library::ChannelManager* m_channelmanager;

};

}

#endif // TCP_HEADER_COVERT_CHANNEL