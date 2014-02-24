// Author: Martin Oehler

#ifndef TIMING_COVERT_CHANNEL
#define TIMING_COVERT_CHANNEL

#include "covertchannel.hpp" 
#include "genericpacket.hpp"
#include "morseCoder.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <udppacket.hpp>

using namespace std;

namespace whisper_library {
	/*
		The TimingCovertChannel implements a covert channel, that uses inter-packet delays to transmit morse. 
		The receiver measures the delays between incoming packets to decode the message. To use the channel, 
		call 'sendMessage' with your message as a string. Received packets are given to 'receiveMessage' which
		returns the decoded message after a timeout of 2 seconds to the callback function m_output.
	*/
class TimingCovertChannel : public CovertChannel {
public:
	/*
		Constructor
		output is a function pointer that is called, when a complete message arrived. Its parameter is this message.
		send is a function pointer that is called to send a UdpPacket via the socket.
		getPacket is a function pointer, that has to return a valid UdpPacket.
	*/
	TimingCovertChannel(function<void(string)> output, function<void(UdpPacket)> send, function<UdpPacket(void)> getPacket)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_getPacket(getPacket),
		m_coder(DELAY_SHORT, DELAY_LONG, DELAY_LETTER, DELAY_SPACE),
		m_receiving(0)
		{};

	/*
		Sends a message using the timing channel. The argument is the message as a string.
	*/
	void sendMessage(string message);

	/*
		Has to be called when a packet is received, with the packet as the argument. 
		After a timeout of 2 seconds the callback m_output is called with the received message.
	*/
	void receiveMessage(GenericPacket& packet);

	// Returns a string with the name of the covert channel "Timing Covert Channel"
	string name() const;

	// Returns a string with basic information about the timing covert channel
	string info() const;

private:
	/*
		Starts the timeout. If m_timeout_changed is false and m_timeout_end is in the past, 
		it calls the callback function m_output.
	*/
	void startTimeoutTimer();

	/*
		Sends Udp packets using m_send with inter-packet delays given as the argument.
	*/
	void sendDelays(vector<unsigned int> delays);

	// MorseCoder is used to encode messages as morse
	MorseCoder m_coder;

	// callback function pointer that is used to return received messages as a string
	function<void(string)> m_output;

	// function pointer that is used to send Udp Packets via the socket
	function<void(UdpPacket)> m_send;

	// function pointer that is used to retrieve valid udp packets, that are send with delay
	function<UdpPacket(void)> m_getPacket;

	// Marks the time the last packet was received to measure inter-packet delays
	chrono::high_resolution_clock::time_point m_receive_start;

	// Marks the time point at which the timeout ends. It is increased by 2 seconds each time a packet is received.
	chrono::high_resolution_clock::time_point m_timeout_end;

	// signals 'startTimeoutTimer' that the timeout end point has changed.
	atomic<bool> m_timeout_changed;

	// stores the measured delays
	vector<unsigned int> m_received_delays;

	// indicates, if the channel is currently receiving a message
	atomic<bool> m_receiving;

	/*
		TODO: calculate delays based on connection
	*/
	// delay_short is used to encode a short signal (in milliseconds)
	static const unsigned int DELAY_SHORT = 10;

	// delay_long is used to encode a long signal (in milliseconds)
	static const unsigned int DELAY_LONG = 30;

	// delay_letter is used to encode the end of a letter (in milliseconds)
	static const unsigned int DELAY_LETTER = 50;

	// delay_space is used to encode space between words (in milliseconds)
	static const unsigned int DELAY_SPACE = 70;
};
}
#endif // TIMING_COVERT_CHANNEL