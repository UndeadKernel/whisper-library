#include "tcppacketgenerator.hpp"
#include <thread>
#include <iostream>

namespace whisper_library {
	TcpPacketGenerator::TcpPacketGenerator(unsigned short port, function<void(TcpPacket)> send, function<void(GenericPacket)> forward)
	: m_send(send),
	  m_forward(forward),
	  m_port(port),
	  m_state(0),
	  m_timeout(1000) {
		m_next_sequence = 0; // TODO randomize
		m_base_sequence = m_next_sequence;
	}

	TcpPacket TcpPacketGenerator::nextPacket() {
		if (m_state == ESTABLISHED) {
			return createPacket(false, false, "A"); // TODO enter data
		}
	}

	TcpPacket TcpPacketGenerator::createPacket(bool syn, bool ack, string data) {
		bitset<4> data_offset("1010");
		uint window_size = 2;
		vector<bool> options;
		ulong ack_number;
		if (ack) {
			ack_number = m_next_peer_sequence;
		}
		else {
			ack_number = 0;
		}
		whisper_library::TcpPacket packet(m_port,
			m_port,
			m_next_sequence,
			ack_number,
			data_offset,
			window_size,
			options);
		packet.setAcknowledgementFlag(ack);
		packet.setSynchronisationFlag(syn);

		vector<bool> binary;
		for (unsigned int i = 0; i < data.length(); i++) {
			char byte = data[i];
			for (unsigned int j = 0; j < 8; j++) {
				unsigned char bit = byte >> (7 - j);
				bit = bit & 0x01;
				if (bit == 0) {
					binary.push_back(false);
				}
				else {
					binary.push_back(true);
				}
			}
		}
		packet.setData(binary);

		m_next_sequence = m_next_sequence + data.length();
		return packet;
	}

	void TcpPacketGenerator::sendConnect() {
		m_send(createPacket(true, false, ""));
		m_next_sequence++;
	}

	void TcpPacketGenerator::sendConnectResponse(){
		m_send(createPacket(true, true, ""));
		m_next_sequence++;
	}
	void TcpPacketGenerator::sendAcknowledgeResponse() {
		m_send(createPacket(false, true, ""));
	}

	void TcpPacketGenerator::receivePacket(TcpPacket packet) {
		if (m_state == NO_CONNECTION) {
			if (packet.synchronisationFlag() && !packet.acknowledgementFlag()) {
				// we are listener, peer wants to connect
				cout << "received connect request" << endl;
				m_next_peer_sequence = packet.sequenceNumber() + 1;
				m_state = RECEIVED_SYN;
				sendConnectResponse();
				return;
			}
			if (packet.synchronisationFlag() && packet.acknowledgementFlag()) {
				// we are sender, reponse of peer to connect request
				cout << "received response to connect request" << endl;
				m_next_peer_sequence = packet.sequenceNumber() + 1;
				m_state = ESTABLISHED;
				// TODO wait for connect-procedure to end
				sendAcknowledgeResponse();
				m_base_sequence = m_next_sequence;
				return;
			}
		}
		if (m_state == RECEIVED_SYN) {
			if (!packet.synchronisationFlag() && packet.acknowledgementFlag()) {
				cout << "received last ack" << endl;
				m_base_sequence = m_next_sequence;
				m_state = ESTABLISHED;
			}
			return;
		}
		if (m_state == ESTABLISHED){
			if (!packet.acknowledgementFlag()) {
				// data packet
				// check if packet is new and in order
				if (packet.sequenceNumber() == m_next_peer_sequence) {
					m_next_peer_sequence = m_next_peer_sequence + (packet.data().size() / 8);
					m_send(createPacket(false, true, ""));
				}
				GenericPacket generic_packet;
				generic_packet.setContent(packet.packet());
				m_forward(generic_packet);
			}
			else {
				// Acknowledgement
				if (packet.acknowlegeNumber() > m_base_sequence) { // accept ack only if it is new
					m_base_sequence = packet.acknowlegeNumber();
				}
			}
		}
	}

	unsigned int TcpPacketGenerator::status() {
		return m_state;
	}
}