// Author: Martin Oehler

#ifndef TIMING_COVERT_CHANNEL
#define TIMING_COVERT_CHANNEL

#include "covertchannel.hpp" 
#include "../../src/tcppacket.hpp"
#include "morseCoder.hpp"
#include <functional>
#include <thread>
#include <atomic>

using namespace std;

namespace whisper_library {
	class TimingCovertChannel : public CovertChannel {
	public:
		TimingCovertChannel(function<void(string)> output, function<void(TcpPacket)> send, function<TcpPacket(void)> getPacket)
			: CovertChannel(),
			m_output(output),
			m_send(send),
			m_getPacket(getPacket),
			m_coder(100, 300, 500, 700),
			m_receiving(0)
			{};

		void sendMessage(string message);

		void receiveMessage(TcpPacket& packet);

		string name();
		string info();

	private:
		void startTimeoutTimer();
		MorseCoder m_coder;
		function<void(string)> m_output;
		function<void(TcpPacket)> m_send;
		function<TcpPacket(void)> m_getPacket;
		chrono::high_resolution_clock::time_point m_start_time;
		chrono::high_resolution_clock::time_point m_timeout_end;
		atomic<bool> m_timeout_changed;
		vector<uint> m_received_delays;
		std::atomic<bool> m_receiving;
		const uint m_delay_short = 100;
		const uint m_delay_long = 300;
		const uint m_delay_letter = 500;
		const uint m_delay_space = 700;
	};
}



#endif // TIMING_COVERT_CHANNEL