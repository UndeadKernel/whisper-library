// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>

using namespace std;

namespace whisper_library {
	TcpHeaderCovertChannel::TcpHeaderCovertChannel(whisper_library::ChannelManager* channelmanager) {
		m_channelmanager = channelmanager;
		m_numb_packets = 0;
	}

	vector<whisper_library::TcpPacket> TcpHeaderCovertChannel::sendMessage(string message) {
		vector<whisper_library::TcpPacket> ret_vector;
		whisper_library::BitSetEncoder encoder;

		vector<bitset<6>> bit_blocks = encoder.encodeMessage(message);
		for (int i = 0; i < bit_blocks.size(); ++i) {
			whisper_library::TcpPacket packet = m_channelmanager->getTcpPacket();
			modifyTcpPacket(packet, bit_blocks[i]);
			ret_vector.push_back(packet);
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
		m_data_blocks.push_back(data);
		if (m_data_blocks.size() == 1) {			// only init-packet received
			m_numb_packets = data.to_ulong() +1;	
		}
		else {
			m_numb_packets--;

			if (m_numb_packets == 0) {
				whisper_library::BitSetDecoder decoder;
				string message = decoder.decodeMessage(m_data_blocks);
				m_channelmanager->outputMessage(message);
				m_data_blocks.clear();			// ready to receive new message
			}
		}
	}

}