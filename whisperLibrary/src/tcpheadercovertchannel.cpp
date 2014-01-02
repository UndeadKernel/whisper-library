// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>

using namespace std;

namespace whisper_library {
	void TcpHeaderCovertChannel::modifyTcpPacket(whisper_library::TcpPacket& packet) {
		bitset<6> message_block = getNextMessageBlock();
		packet.set_reserved(message_block.to_ulong());
	}

	void TcpHeaderCovertChannel::receiveMessage(whisper_library::TcpPacket& packet) {
		bitset<6> message_block(packet.reserved());
		m_received_message_blocks.push_back(message_block);

		whisper_library::BitSetDecoder decoder;
		m_output_string = decoder.decodeMessage(m_received_message_blocks);
	}

	bitset<6> TcpHeaderCovertChannel::getNextMessageBlock() {
		bitset<6> next_message_block = m_message_blocks.front();
		m_message_blocks.erase(m_message_blocks.begin());
		return next_message_block;
	}

	void TcpHeaderCovertChannel::addMessage(string message) {
		whisper_library::BitSetEncoder encoder;
		vector<bitset<6>> new_message_blocks = encoder.encodeMessage(message);
		m_message_blocks.insert(m_message_blocks.end(), new_message_blocks.begin(), new_message_blocks.end());
	}
}