// Author: Martin Oehler

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include <tcppacket.hpp>
#include <string>
#include <vector>
#include <bitset>
#include <channelmanager.hpp>
#include <bitSetCoder.hpp>

using namespace std;

namespace whisper_library {

class TcpHeaderCovertChannel : public CovertChannel {

public:
	TcpHeaderCovertChannel(whisper_library::ChannelManager* channelmanager);
	void sendMessage(string message);
	void receiveMessage(whisper_library::TcpPacket& packet);

private:
	vector<bitset<3>> encodeMessageWithLength(string message);
	void modifyTcpPacket(whisper_library::TcpPacket& packet, bitset<3> data);
	bitset<3> extractData(whisper_library::TcpPacket& packet);
	vector<bitset<3>> m_data_blocks;
	int m_numb_packets;
	whisper_library::ChannelManager* m_channelmanager;
	whisper_library::BitSetCoder<3> m_coder;

};

}

#endif // TCP_HEADER_COVERT_CHANNEL