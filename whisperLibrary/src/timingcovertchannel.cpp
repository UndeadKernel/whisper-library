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
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <cmath>

namespace whisper_library {
	TimingCovertChannel::~TimingCovertChannel() {
		m_mutex_sending.lock();
		delete m_coder;
		m_mutex_sending.unlock();
	}

	CovertChannel* TimingCovertChannel::instance(function<void(string)> output,
		function<void(UdpPacket)> send,
		function<UdpPacket(unsigned short)> getPacket){
		return new TimingCovertChannel(output, send, getPacket);
	}

	string TimingCovertChannel::name() const{
		return "Timing Covert Channel";
	}

	string TimingCovertChannel::info() const{
		return "This covert channel uses inter-packet delays to transmit morse code.";
	}

	string TimingCovertChannel::protocol() const {
		return "udp";
	}

	unsigned short TimingCovertChannel::port() const{
		return 23;
	}

	void TimingCovertChannel::sendMessage(string message) {
		if (message.empty()) {
			return;
		}
		vector<unsigned int> delays = m_coder->encodeMessage(message);
		thread send_delays(bind(&TimingCovertChannel::sendDelays,this,delays));
		send_delays.detach();
	}

	void TimingCovertChannel::sendDelays(vector<unsigned int> delays) {
		m_mutex_sending.lock();
		chrono::time_point<chrono::system_clock> sending_time;
		m_send(m_getPacket(port()));
		chrono::time_point<chrono::system_clock> start = chrono::system_clock::now();
		for (vector<unsigned int>::iterator it = delays.begin(); it != delays.end(); it++) {
			UdpPacket packet = m_getPacket(port());
			sending_time = start + chrono::milliseconds(*it);
			this_thread::sleep_until(sending_time);
			m_send(packet);
			start = chrono::system_clock::now();
		}
		this_thread::sleep_for(chrono::milliseconds(m_timeout));
		m_mutex_sending.unlock();
	}

	void TimingCovertChannel::receivePacket(GenericPacket& packet){
		// update timeout point
		m_timeout_changed = true;	
		m_timeout_end = chrono::system_clock::now() + chrono::milliseconds(m_timeout);
		if (!m_receiving) {
			// first packet arrived
			m_receive_start = chrono::system_clock::now();
			m_receiving = true;
			// start timeout
			std::thread timeout(bind(&TimingCovertChannel::startTimeoutTimer, this));
			timeout.detach();
		}
		else {
			// measure time since last packet
			chrono::time_point<chrono::system_clock> end = chrono::system_clock::now();
			chrono::duration<unsigned int, milli> time_elapsed;
			time_elapsed = chrono::duration_cast<chrono::duration<unsigned int, milli>>(end - m_receive_start);
			m_receive_start = end;
			unsigned int delay = time_elapsed.count();
			// check which delay was received
			if (delay < m_threshold_delay_short) {
				m_received_delays.push_back(m_delay_short);
			}
			else {
				if (delay < m_threshold_delay_long) {
					m_received_delays.push_back(m_delay_long);
				}
				else {
					if (delay < m_threshold_delay_letter) {
						m_received_delays.push_back(m_delay_letter);
					}
					else {
						m_received_delays.push_back(m_delay_space);
					}
				}
			}
		}
	}

	void TimingCovertChannel::setArguments(string arguments) {
		vector<string> parts;
		boost::split(parts, arguments, boost::is_any_of(" "), boost::token_compress_on);
		if (parts[0].compare("-set_timings") == 0 && parts.size() == 5) {
			m_delay_short = boost::lexical_cast<unsigned int>(parts[1]);
			m_delay_long = boost::lexical_cast<unsigned int>(parts[2]);
			m_delay_letter = boost::lexical_cast<unsigned int>(parts[3]);
			m_delay_space = boost::lexical_cast<unsigned int>(parts[4]);
			calculateTresholds();
			delete m_coder;
			m_coder = new MorseCoder(m_delay_short, m_delay_long, m_delay_letter, m_delay_space);
		}
		else {
			cerr << "Couldn't parse argument string" << endl;
		}
	}

	void TimingCovertChannel::setOutput(function<void(string)> output){
		m_output = output;
	}

	void TimingCovertChannel::startTimeoutTimer() {
		// wait until timeout point, repeat if timeout was changed during sleep
		while (m_timeout_changed) {
			m_timeout_changed = false;
			this_thread::sleep_until(m_timeout_end);
		}
		// all packets received (timed out)
		m_receiving = false;
		string message = m_coder->decodeMessage(m_received_delays);
		m_received_delays.clear();
		m_output(message);
	}

	void TimingCovertChannel::calculateTresholds() {
		m_threshold_delay_short = (m_delay_long + m_delay_short) / 2;
		m_threshold_delay_long = (m_delay_long + m_delay_letter) / 2;
		m_threshold_delay_letter = (m_delay_letter + m_delay_space) / 2;
		m_timeout = static_cast<unsigned int>(round(m_delay_space*1.5));
	}
}
