/*	<timingcovertchannel.cpp>
	Copyright(C) 2014   Jan Simon Bunten
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
#include <timingcovertchannel.hpp>

namespace whisper_library {
	
	// delay_short is used to encode a short signal (in milliseconds)
	const unsigned int TimingCovertChannel::DELAY_SHORT = 10;

	// delay_long is used to encode a long signal (in milliseconds)
	const unsigned int TimingCovertChannel::DELAY_LONG = 30;

	// delay_letter is used to encode the end of a letter (in milliseconds)
	const unsigned int TimingCovertChannel::DELAY_LETTER = 50;

	// delay_space is used to encode space between words (in milliseconds)
	const unsigned int TimingCovertChannel::DELAY_SPACE = 80;
	
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

	void TimingCovertChannel::receivePacket(GenericPacket& packet){
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
			chrono::duration<unsigned int, milli> time_elapsed;
			time_elapsed = chrono::duration_cast<chrono::duration<unsigned int, milli>>(end - m_receive_start);
			m_receive_start = end;
			unsigned int delay = time_elapsed.count();
			// check which delay was received
			if (delay < TimingCovertChannel::DELAY_LONG) {
				m_received_delays.push_back(TimingCovertChannel::DELAY_SHORT);
			}
			else {
				if (delay < TimingCovertChannel::DELAY_LETTER) {
					m_received_delays.push_back(TimingCovertChannel::DELAY_LONG);
				}
				else {
					if (delay < TimingCovertChannel::DELAY_SPACE) {
						m_received_delays.push_back(TimingCovertChannel::DELAY_LETTER);
					}
					else {
						m_received_delays.push_back(TimingCovertChannel::DELAY_SPACE);
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
