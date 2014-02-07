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
		cout << "TimingCovertChannel: packet received: ";
		m_timeout_changed = true;
		m_timeout_end = chrono::high_resolution_clock::now() + chrono::seconds(2);
		if (!m_receiving) {
			cout << "first packet" << endl;
			m_receiving = true;
			m_start_time = chrono::high_resolution_clock::now();
			std::thread timeout(bind(&TimingCovertChannel::startTimeoutTimer, this));
			timeout.detach();
		}
		else {
			chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
			chrono::duration<uint, milli> time_elapsed = chrono::duration_cast<chrono::duration<uint, milli>>(end - m_start_time);
			uint delay = time_elapsed.count();
			if (delay < m_delay_long) {
				m_received_delays.push_back(m_delay_short);
				cout << "delay short" << endl;
			}
			else {
				if (delay < m_delay_letter) {
					m_received_delays.push_back(m_delay_long);
					cout << "delay long" << endl;
				}
				else {
					if (delay < m_delay_space) {
						m_received_delays.push_back(m_delay_letter);
						cout << "delay letter" << endl;
					}
					else {
						m_received_delays.push_back(m_delay_space);
						cout << "delay space" << endl;
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
		cout << "timeout started" << endl;
		while (m_timeout_changed) {
			m_timeout_changed = false;
			this_thread::sleep_until(m_timeout_end);
		}
		cout << "timeout finished!" << endl;
		m_receiving = false;
		string message = m_coder.decodeMessage(m_received_delays);
		m_received_delays.clear();
		cout << "Timing Covert Channel: Received Message: ";
		m_output(message);
	}
}