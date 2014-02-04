#include <timingcovertchannel.hpp>
#include <thread>

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
		
	}

	string TimingCovertChannel::name() {
		return "Timing Covert Channel";
	}

	string TimingCovertChannel::info() {
		return "blub";
	}
}