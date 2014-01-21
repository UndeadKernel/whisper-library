// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>

using namespace std;

namespace whisper_library {
	void TcpHeaderCovertChannel::sendMessage(string message) {
		vector<bitset<3>> bit_blocks = encodeMessageWithLength(message);

		//iterate through blocks and modify packets
		for (int i = 0; i < bit_blocks.size(); ++i) {
			whisper_library::TcpPacket packet = m_getPacket();
			modifyTcpPacket(packet, bit_blocks[i]);
			
			m_send(packet);
		}

	}

	void TcpHeaderCovertChannel::receiveMessage(whisper_library::TcpPacket& packet) {
		bitset<3> data = extractData(packet);
		if (m_numb_packets == 0) {					// no message received yet
			m_numb_packets = data.to_ulong() + 1;	// save length from first packet
		}
		else {
			m_data_blocks.push_back(data);
			m_numb_packets--;

			if (m_numb_packets == 0) {				// all messages received
				string message = m_coder.decodeMessage(m_data_blocks);
				m_output(message);
				m_data_blocks.clear();			// ready to receive new message
			}
		}
	}

	vector<bitset<3>> TcpHeaderCovertChannel::encodeMessageWithLength(string message) {
		vector<bitset<3>> ret_vector;
		if (message.length() <= 24) {
			ret_vector = m_coder.encodeMessage(message);
		}
		else {
			string head = message.substr(0, 24);	//length of 24 chars
			string rest = message.substr(24, message.length() - 24);	// length > 0
			ret_vector = m_coder.encodeMessage(head);
			vector<bitset<3>> rec_vector = encodeMessageWithLength(rest);	//recursive call
			ret_vector.insert(ret_vector.end(), rec_vector.begin(), rec_vector.end());
		}
		return ret_vector;
	}

	void TcpHeaderCovertChannel::modifyTcpPacket(whisper_library::TcpPacket& packet, bitset<3> data) {
		packet.setReserved(data);
	}

	bitset<3> TcpHeaderCovertChannel::extractData(whisper_library::TcpPacket& packet) {
		return packet.reserved();
	}

}