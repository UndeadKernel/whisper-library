// Author: Martin Oehler

#ifndef TIMING_COVERT_CHANNEL
#define TIMING_COVERT_CHANNEL

#include "covertchannel.hpp" 
#include "../../src/tcppacket.hpp"
#include "morseCoder.hpp"
#include <functional>

using namespace std;

namespace whisper_library {
	class TimingCovertChannel : public CovertChannel {
	public:
		TimingCovertChannel(function<void(string)> output, function<void(TcpPacket)> send, function<TcpPacket(void)> getPacket)
			: CovertChannel(),
			m_output(output),
			m_send(send),
			m_getPacket(getPacket),
			m_coder(100, 300, 500, 700)
			{};

		void sendMessage(string message);

		void receiveMessage(TcpPacket& packet);

		string name();
		string info();

	private:
		MorseCoder m_coder;
		function<void(string)> m_output;
		function<void(TcpPacket)> m_send;
		function<TcpPacket(void)> m_getPacket;
	};
}



#endif // TIMING_COVERT_CHANNEL