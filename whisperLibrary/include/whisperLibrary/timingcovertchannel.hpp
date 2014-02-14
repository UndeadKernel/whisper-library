// Author: Martin Oehler

#ifndef TIMING_COVERT_CHANNEL
#define TIMING_COVERT_CHANNEL

#include "covertchannel.hpp" 
#include <genericpacket.hpp>
#include "morseCoder.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <udppacket.hpp>

using namespace std;

namespace whisper_library {
	class TimingCovertChannel : public CovertChannel {
	public:
		TimingCovertChannel(function<void(string)> output, function<void(UdpPacket)> send, function<UdpPacket(void)> getPacket)
			: CovertChannel(),
			m_output(output),
			m_send(send),
			m_getPacket(getPacket),
			m_coder(100, 300, 500, 700),
			m_receiving(0)
			{};

		void sendMessage(string message);

		void receiveMessage(GenericPacket& packet);

		string name();
		string info();

	private:
		void startTimeoutTimer();
		void sendDelays(vector<unsigned int> delays);
		MorseCoder m_coder;
		function<void(string)> m_output;
		function<void(UdpPacket)> m_send;
		function<UdpPacket(void)> m_getPacket;
		chrono::high_resolution_clock::time_point m_receive_start;
		chrono::high_resolution_clock::time_point m_timeout_end;
		atomic<bool> m_timeout_changed;
		vector<unsigned int> m_received_delays;
		std::atomic<bool> m_receiving;
		const unsigned int m_delay_short = 100;
		const unsigned int m_delay_long = 300;
		const unsigned int m_delay_letter = 500;
		const unsigned int m_delay_space = 700;
	};
}



#endif // TIMING_COVERT_CHANNEL