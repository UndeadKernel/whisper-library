#include <timingcovertchannel.hpp>

namespace whisper_library {
	void TimingCovertChannel::sendMessage(string message) {
		vector<unsigned int> delays = m_coder.encodeMessage(message);

		chrono::high_resolution_clock::time_point sending_time;
		chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
		m_send(m_getPacket());
		unsigned int total_delay = 0;
		for (vector<unsigned int>::iterator it = delays.begin(); it != delays.end(); it++) {
			TcpPacket packet = m_getPacket();
			total_delay += (*it);
			sending_time = start + chrono::milliseconds(total_delay);
			this_thread::sleep_until(sending_time);
			m_send(packet);
		}
	}

	void TimingCovertChannel::receiveMessage(TcpPacket& packet){
		if (!m_receiving) {
			m_start_time = chrono::high_resolution_clock::now();
			std::thread timeout(bind(&TimingCovertChannel::startTimeoutTimer, this, 7000));
		}
		else {
			m_receiving = true;
			chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
			chrono::duration<uint, milli> time_elapsed = chrono::duration_cast<chrono::duration<uint, milli>>(end - m_start_time);
			uint delay = time_elapsed.count();
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

	void TimingCovertChannel::startTimeoutTimer(uint timeout_ms) {
		this_thread::sleep_for(chrono::milliseconds(timeout_ms));
		m_receiving = false;
	}
}