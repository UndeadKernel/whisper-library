/*	<tcpheadercovertchannel.cpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <tcpheadercovertchannel.hpp>
#include <iostream>

using namespace std;

namespace whisper_library {

	TcpHeaderCovertChannel::TcpHeaderCovertChannel(function<void(string)> output, function<void(GenericPacket)> send)
		: CovertChannel(),
		m_remaining_packets(0),
		m_output(output),
		m_send(send) {
		m_generator = new TcpPacketGenerator(port(), m_send);
	};

	TcpHeaderCovertChannel::TcpHeaderCovertChannel()
		: CovertChannel(),
		m_remaining_packets(0),
		m_generator(NULL) {
	};

	TcpHeaderCovertChannel::~TcpHeaderCovertChannel(){
		delete m_generator;
	}

	string TcpHeaderCovertChannel::name() const{
		return "TCP Header Covert Channel";
	}

	string TcpHeaderCovertChannel::info() const{
		return	"The TCP Header Covert Channel uses unused bits in the TCP Header to hide data. 3 bits are sent per packet.";
	}

	string TcpHeaderCovertChannel::protocol() const {
		return "tcp";
	}

	unsigned short TcpHeaderCovertChannel::port() const {
		return 8080;
	}

	string TcpHeaderCovertChannel::id() const {
		return "tcp_header_channel";
	}

	CovertChannel* TcpHeaderCovertChannel::instance(){
		return new TcpHeaderCovertChannel();
	}

	void TcpHeaderCovertChannel::initialize() {
		if (m_generator != NULL) {
			m_generator->sendConnect();
		}
	}

	void TcpHeaderCovertChannel::sendMessage(string message) {
		vector<bitset<3>> bit_blocks = encodeMessageWithLength(message);
		for (uint i = 0; i < bit_blocks.size(); ++i) {			//iterate through blocks and modify packets
			TcpPacket packet = m_generator->nextPacket();	// get valid tcp packet
			modifyTcpPacket(packet, bit_blocks[i]);	
			GenericPacket g_packet;
			g_packet.setPacket(packet.packet());
			m_send(g_packet);
		}
	}

	void TcpHeaderCovertChannel::receivePacket(GenericPacket& packet) {
		TcpPacket tcp_packet;
		tcp_packet.setPacket(packet.packet());
		bool data_packet = m_generator->receivePacket(tcp_packet);
		if (!data_packet) {return;}
		bitset<3> data = extractData(tcp_packet);
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

	void TcpHeaderCovertChannel::setOutput(function<void(string)> output){
		m_output = output;
	}

	void TcpHeaderCovertChannel::setSend(function<void(GenericPacket)> send){
		m_send = send;
		if (m_generator == NULL) {
			m_generator = new TcpPacketGenerator(port(), send);
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
