#include <timingcovertchannel.hpp>

namespace whisper_library {
	void TimingCovertChannel::sendMessage(string message) {
		vector<unsigned int> delays = m_coder.encodeMessage(message);
		thread send_delays(bind(&TimingCovertChannel::sendDelays,this,delays));
		send_delays.detach();
	}

	void TimingCovertChannel::sendDelays(vector<unsigned int> delays) {
		chrono::high_resolution_clock::time_point sending_time;
		m_send(m_getPacket());
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		for (vector<unsigned int>::iterator it = delays.begin(); it != delays.end(); it++) {
			UdpPacket packet = m_getPacket();
			sending_time = start + chrono::milliseconds(*it);
			this_thread::sleep_until(sending_time);
			m_send(packet);
			start = chrono::high_resolution_clock::now();
		}
	}

	void TimingCovertChannel::receiveMessage(GenericPacket& packet){
		m_timeout_changed = true;
		m_timeout_end = chrono::high_resolution_clock::now() + chrono::seconds(2);
		if (!m_receiving) {
			m_receive_start = chrono::high_resolution_clock::now();
			m_receiving = true;
			std::thread timeout(bind(&TimingCovertChannel::startTimeoutTimer, this));
			timeout.detach();
		}
		else {
			chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
			chrono::duration<unsigned int, milli> time_elapsed = chrono::duration_cast<chrono::duration<unsigned int, milli>>(end - m_receive_start);
			m_receive_start = end;
			unsigned int delay = time_elapsed.count();
			if (delay < m_delay_long) {
				m_received_delays.push_back(m_delay_short);
			}
			else {
				if (delay < m_delay_letter) {
					m_received_delays.push_back(m_delay_long);
				}
				else {
					if (delay < m_delay_space) {
						m_received_delays.push_back(m_delay_letter);
					}
					else {
						m_received_delays.push_back(m_delay_space);
					}
				}
			}
		}
	}

	string TimingCovertChannel::name() {
		return "Timing Covert Channel";
	}

	string TimingCovertChannel::info() {
		return "blub";
	}

	void TimingCovertChannel::startTimeoutTimer() {
		while (m_timeout_changed) {
			m_timeout_changed = false;
			this_thread::sleep_until(m_timeout_end);
		}
		m_receiving = false;
		string message = m_coder.decodeMessage(m_received_delays);
		m_received_delays.clear();
		m_output(message);
	}
}