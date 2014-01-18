// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>

using namespace std;

namespace whisper_library {
	TcpHeaderCovertChannel::TcpHeaderCovertChannel(whisper_library::ChannelManager* channelmanager) {
		m_channelmanager = channelmanager;
		m_numb_packets = 0;
	}

	void TcpHeaderCovertChannel::sendMessage(string message) {
		vector<whisper_library::TcpPacket> ret_vector;

		vector<bitset<6>> bit_blocks = encodeMessageWithLength(message);

		//iterate through blocks and modify packets
		for (int i = 0; i < bit_blocks.size(); ++i) {
			whisper_library::TcpPacket packet = m_channelmanager->getTcpPacket();
			modifyTcpPacket(packet, bit_blocks[i]);
			ret_vector.push_back(packet);
		}

		//TODO send packets with socketconnector
	}

	vector<bitset<6>> TcpHeaderCovertChannel::encodeMessageWithLength(string message) {
		vector<bitset<6>> ret_vector;
		if (message.length() <= 48) {
			ret_vector = m_coder.encodeMessage(message);
		}
		else {
			string head = message.substr(0, 48);	//length of 48 chars
			string rest = message.substr(48, message.length() - 48);	// length > 0
			ret_vector = m_coder.encodeMessage(head);
			vector<bitset<6>> rec_vector = encodeMessageWithLength(rest);	//recursive call
			ret_vector.insert(ret_vector.end(), rec_vector.begin(), rec_vector.end());
		}
		return ret_vector;
	}

	void TcpHeaderCovertChannel::modifyTcpPacket(whisper_library::TcpPacket& packet, bitset<6> data) {
		packet.set_reserved(data.to_ulong());
	}

	bitset<6> TcpHeaderCovertChannel::extractData(whisper_library::TcpPacket& packet) {
		return bitset<6>(packet.reserved());
	}

	void TcpHeaderCovertChannel::receiveMessage(whisper_library::TcpPacket& packet) {
		bitset<6> data = extractData(packet);
		if (m_numb_packets == 0) {					// no message received yet
			m_numb_packets = data.to_ulong() +1;	// save length from first packet
		}
		else {
			m_data_blocks.push_back(data);
			m_numb_packets--;

			if (m_numb_packets == 0) {				// all messages received
				string message = m_coder.decodeMessage(m_data_blocks);
				m_channelmanager->outputMessage(message);
				m_data_blocks.clear();			// ready to receive new message
			}
		}
	}

}