// Author: Martin Oehler
#include <tcpheadercovertchannel.hpp>
#include <iostream>

using namespace std;

namespace whisper_library {
	string TcpHeaderCovertChannel::name() const{
		return "TCP Header Covert Channel";
	}

	string TcpHeaderCovertChannel::info() const{
		return	"The TCP Header Covert Channel uses unused bits in the TCP Header to hide data. 3 bits are sent per packet.";
	}

	string TcpHeaderCovertChannel::protocol() const {
		return "tcp";
	}

	void TcpHeaderCovertChannel::sendMessage(string message) {
		vector<bitset<3>> bit_blocks = encodeMessageWithLength(message);
		for (uint i = 0; i < bit_blocks.size(); ++i) {			//iterate through blocks and modify packets
			whisper_library::TcpPacket packet = m_getPacket();	// get valid tcp packet
			modifyTcpPacket(packet, bit_blocks[i]);			
			m_send(packet);
		}
	}

	void TcpHeaderCovertChannel::receiveMessage(whisper_library::GenericPacket& packet) {
		TcpPacket tcp_packet;
		tcp_packet.setPacket(packet.content());
		bitset<3> data = extractData(tcp_packet);
		//cout << "received: " << data << endl;
		if (m_remaining_packets == 0) {					// no message received yet
			m_remaining_packets = data.to_ulong() + 1;	// save length from first packet
			return;
		}
		// process messages
		m_data_blocks.push_back(data);
		m_remaining_packets--;

		if (m_remaining_packets == 0) {				// all packets received
			string message = m_coder.decodeMessage(m_data_blocks);
			m_output(message);
			m_data_blocks.clear();					// ready to receive new message
		}
	}

	vector<bitset<3>> TcpHeaderCovertChannel::encodeMessageWithLength(string message) {
		vector<bitset<3>> ret_vector;
		if (message.length() == 0) {
			return ret_vector;
		}
		if (message.length() <= 3) {
			ret_vector = m_coder.encodeMessage(message);
			ret_vector.insert(ret_vector.begin(), bitset<3>(ret_vector.size()-1));	// insert length block at front
		}
		else {
			string head = message.substr(0, 3);						//length of 3 chars
			string rest = message.substr(3, message.length() - 3);	// length > 0
			ret_vector = m_coder.encodeMessage(head);
			ret_vector.insert(ret_vector.begin(), bitset<3>(ret_vector.size() - 1));
			vector<bitset<3>> rec_vector = encodeMessageWithLength(rest);				//recursive call
			ret_vector.insert(ret_vector.end(), rec_vector.begin(), rec_vector.end());	// return head + encode(rest)
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