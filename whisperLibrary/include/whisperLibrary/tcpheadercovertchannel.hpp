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
#include <covertchannel.hpp>

using namespace std;

namespace whisper_library {

class TcpHeaderCovertChannel : public CovertChannel {

public:
	TcpHeaderCovertChannel(ChannelManager* channelmanager)
		: CovertChannel(channelmanager), m_numb_packets(0), m_channelmanager(channelmanager) {};
	void sendMessage(string message);
	void receiveMessage(TcpPacket& packet);

private:
	vector<bitset<3>> encodeMessageWithLength(string message);
	void modifyTcpPacket(TcpPacket& packet, bitset<3> data);
	bitset<3> extractData(TcpPacket& packet);
	vector<bitset<3>> m_data_blocks;
	int m_numb_packets;
	BitSetCoder<3> m_coder;
	ChannelManager* m_channelmanager;

};

}

#endif // TCP_HEADER_COVERT_CHANNEL