#include "tcppacketgenerator.hpp"
#include <thread>

namespace whisper_library {
	TcpPacketGenerator::TcpPacketGenerator(unsigned short port, function<void(TcpPacket)> send, function<void(GenericPacket)> forward)
	: m_send(send),
	  m_forward(forward),
	  m_port(port),
	  m_state(0),
	  m_timeout(1000) {
		m_send_sequence = 0; // TODO randomize
		m_base_sequence = m_send_sequence;
	}

	TcpPacket TcpPacketGenerator::nextPacket() {
		if (m_state = NO_CONNECTION) {
			// not good
		}

	}

	void TcpPacketGenerator::sendConnect() {
		bitset<4> dataOffset("1010");
		ulong acknowledge = 0;
		uint window_size = 2;
		vector<bool> options;
		whisper_library::TcpPacket packet(m_port,
			m_port,
			m_send_sequence,
			acknowledge,
			dataOffset,
			window_size,
			options);
		packet.setAcknowledgementFlag(0);
		packet.setSynchronisationFlag(1);


		while (m_state == NO_CONNECTION) {
			m_send(packet);
			this_thread::sleep_for(chrono::milliseconds(m_timeout));
		}

		// we are sender
		if (m_state == ESTABLISHED) {
			m_send_sequence++;
		}
	}

	void TcpPacketGenerator::sendConnectResponse(){
		m_send_sequence++;
		bitset<4> dataOffset("1010");
		ulong acknowledge = m_peer_sequence;
		uint window_size = 2;
		vector<bool> options;
		whisper_library::TcpPacket packet(m_port,
			m_port,
			m_send_sequence,
			acknowledge,
			dataOffset,
			window_size,
			options);
		packet.setAcknowledgementFlag(1);
		packet.setSynchronisationFlag(1);

		m_send(packet);
	}
	void TcpPacketGenerator::sendAcknowledgeResponse() {
		m_send_sequence++;
		bitset<4> dataOffset("1010");
		ulong acknowledge = m_peer_sequence + 1;
		uint window_size = 2;
		vector<bool> options;
		whisper_library::TcpPacket packet(m_port,
			m_port,
			m_send_sequence,
			acknowledge,
			dataOffset,
			window_size,
			options);
		packet.setAcknowledgementFlag(1);

		m_send(packet);
	}

	void TcpPacketGenerator::receivePacket(TcpPacket packet) {
		if (m_state == NO_CONNECTION) {
			if (packet.synchronisationFlag() && !packet.acknowledementFlag()) {
				// we are listener, peer wants to connect
				m_peer_sequence = packet.sequenceNumber()+1;
				sendConnectResponse();
				m_state = RECEIVED_SYN;
				return;
			}
			if (packet.synchronisationFlag() && packet.acknowledementFlag()) {
				// we are sender, reponse of peer to connect request
				m_peer_sequence = packet.sequenceNumber() +1;
				m_base_sequence = packet.acknowlegeNumber()+1; // last handshake message isn't acknowledged
				m_state = ESTABLISHED;
				// TODO wait for connect-procedure to end
				sendAcknowledgeResponse();
				return;
			}
		}
		if (m_state == RECEIVED_SYN) {
			if (!packet.synchronisationFlag() && packet.acknowledementFlag()) {
				m_base_sequence = packet.acknowlegeNumber();
				m_state = ESTABLISHED;
			}
		}
	}
}