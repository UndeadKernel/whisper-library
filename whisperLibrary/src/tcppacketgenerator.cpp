#include "tcppacketgenerator.hpp"
#include <thread>
#include <iostream>
#include <random>

namespace whisper_library {
	TcpPacketGenerator::TcpPacketGenerator(unsigned short port, function<void(GenericPacket)> send)
	: m_send(send),
	  m_port(port),
	  m_state(0),
	  m_timeout(1000),
	  m_server(false) {
		try {
			random_device generator;
			uniform_int_distribution<int> distribution(0, 1000);
			m_next_sequence = distribution(generator);
		}
		catch (exception) {
			// true random numbers not supported
			default_random_engine generator;
			uniform_int_distribution<int> distribution(0, 1000);
			m_next_sequence = distribution(generator);
		}

		m_base_sequence = m_next_sequence;
	}

	TcpPacket TcpPacketGenerator::nextPacket() {
		if (m_state == ESTABLISHED) {
			string http_text;
			if (!m_server) {
				http_text = "GET /index.html HTTP/1.1\n"
							"Host: www.google.com";
			}
			else {
				http_text = "HTTP/1.1 404 Not Found";
			}
			return createPacket(false, false, http_text);
		}
		else {
			// no connection, send synchronisation packet
			return createPacket(true, false, "");
		}
	}

	TcpPacket TcpPacketGenerator::createPacket(bool syn, bool ack, string data) {
		bitset<4> data_offset("1010"); // 5
		uint window_size = 15360; // 16 Kilobyte
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

		packet.setData(data);

		m_next_sequence = m_next_sequence + data.length();
		return packet;
	}

	void TcpPacketGenerator::sendConnect() {
		GenericPacket packet;
		packet.setPacket(createPacket(true, false, "").packet());
		m_send(packet);
		m_next_sequence++;
	}

	void TcpPacketGenerator::sendConnectResponse(){
		GenericPacket packet;
		packet.setPacket(createPacket(true, true, "").packet());
		m_send(packet);
		m_next_sequence++;
	}
	void TcpPacketGenerator::sendAcknowledgeResponse() {
		GenericPacket packet;
		packet.setPacket(createPacket(false, true, "").packet());
		m_send(packet);
	}

	void TcpPacketGenerator::receivePacket(TcpPacket packet) {
		if (m_state == NO_CONNECTION) {
			if (packet.synchronisationFlag() && !packet.acknowledgementFlag()) {
				// we are listener, peer wants to connect
				m_server = true;
				m_next_peer_sequence = packet.sequenceNumber() + 1;
				m_state = RECEIVED_SYN;
				sendConnectResponse();
				return;
			}
			if (packet.synchronisationFlag() && packet.acknowledgementFlag()) {
				// we are sender, reponse of peer to connect request
				m_server = false;
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
				m_base_sequence = m_next_sequence;
				m_state = ESTABLISHED;
			}
			return;
		}
		if (m_state == ESTABLISHED){
			if (!packet.acknowledgementFlag()) {
				// data packet
				//forward to covert channel
				GenericPacket generic_packet;
				generic_packet.setPacket(packet.packet());
				// check if packet is new and in order
				if (packet.sequenceNumber() == m_next_peer_sequence) {
					m_next_peer_sequence += (packet.data().size() / 8);
					GenericPacket packet;
					packet.setPacket(createPacket(false, true, "").packet());
					m_send(packet);
				}
			}
			else {
				// Acknowledgement
				if (packet.acknowledgeNumber() > m_base_sequence) { // accept ack only if it is new
					m_base_sequence = packet.acknowledgeNumber();
				}
			}
		}
	}

	unsigned int TcpPacketGenerator::status() {
		return m_state;
	}
}