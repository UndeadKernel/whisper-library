#include <timingcovertchannel.hpp>

namespace whisper_library {
	string TimingCovertChannel::name() const{
		return "Timing Covert Channel";
	}

	string TimingCovertChannel::info() const{
		return "This covert channel uses inter-packet delays to transmit morse code.";
	}

	void TimingCovertChannel::sendMessage(string message) {
		if (message.empty()) {
			return;
		}
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
		// update timeout point
		m_timeout_changed = true;	
		m_timeout_end = chrono::high_resolution_clock::now() + chrono::seconds(2);
		if (!m_receiving) {
			// first packet arrived
			m_receive_start = chrono::high_resolution_clock::now();
			m_receiving = true;
			// start timeout
			std::thread timeout(bind(&TimingCovertChannel::startTimeoutTimer, this));
			timeout.detach();
		}
		else {
			// measure time since last packet
			chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
			chrono::duration<unsigned int, milli> time_elapsed = chrono::duration_cast<chrono::duration<unsigned int, milli>>(end - m_receive_start);
			m_receive_start = end;
			unsigned int delay = time_elapsed.count();
			// check which delay was received
			if (delay < DELAY_LONG) {
				m_received_delays.push_back(DELAY_SHORT);
			}
			else {
				if (delay < DELAY_LETTER) {
					m_received_delays.push_back(DELAY_LONG);
				}
				else {
					if (delay < DELAY_SPACE) {
						m_received_delays.push_back(DELAY_LETTER);
					}
					else {
						m_received_delays.push_back(DELAY_SPACE);
					}
				}
			}
		}
	}

	void TimingCovertChannel::startTimeoutTimer() {
		// wait until timeout point, repeat if timeout was changed during sleep
		while (m_timeout_changed) {
			m_timeout_changed = false;
			this_thread::sleep_until(m_timeout_end);
		}
		// all packets received (timed out)
		m_receiving = false;
		string message = m_coder.decodeMessage(m_received_delays);
		m_received_delays.clear();
		m_output(message);
	}
}