// Author: Martin Oehler

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include "tcppacket.hpp"
#include <string>
#include <vector>
#include <bitset>
#include "bitSetCoder.hpp"
#include "covertchannel.hpp"
#include <functional>

using namespace std;

namespace whisper_library {


class TcpHeaderCovertChannel : public CovertChannel {

public:
	TcpHeaderCovertChannel(function<void(string)> output, function<void(TcpPacket)> send, function<TcpPacket(void)> getPacket)
		: CovertChannel(),
			m_numb_packets(0), 
			m_output(output), 
			m_send(send),
			m_getPacket(getPacket) {};

	void sendMessage(string message);
	void receiveMessage(TcpPacket& packet);

private:
	vector<bitset<3>> encodeMessageWithLength(string message);
	void modifyTcpPacket(TcpPacket& packet, bitset<3> data);
	bitset<3> extractData(TcpPacket& packet);
	vector<bitset<3>> m_data_blocks;
	int m_numb_packets;
	BitSetCoder<3> m_coder;
	function<void(string)> m_output;
	function<void(TcpPacket)> m_send;
	function<TcpPacket(void)> m_getPacket;

};

}

#endif // TCP_HEADER_COVERT_CHANNEL