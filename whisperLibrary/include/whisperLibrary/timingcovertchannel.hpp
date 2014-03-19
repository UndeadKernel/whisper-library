/*	<timingcovertchannel.hpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
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

#ifndef TIMING_COVERT_CHANNEL
#define TIMING_COVERT_CHANNEL

#include "covertchannel.hpp" 
#include "genericpacket.hpp"
#include "morseCoder.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include "../../src/udppacket.hpp"

using namespace std;

namespace whisper_library {
	/** \brief A covert channel using inter packet delays

		The TimingCovertChannel implements a covert channel, that uses inter-packet 
		delays to transmit morse. The receiver measures the delays between incoming 
		packets to decode the message. To use the channel, call 'sendMessage' with 
		your message as a string. Received packets are given to 'receiveMessage' 
		which returns the decoded message after a timeout of 2 seconds to the 
		callback function m_output.
	*/
class TimingCovertChannel : public CovertChannel {
public:
	/*
		TODO: calculate delays based on connection
	*/
	static const unsigned int DELAY_SHORT;///< used to encode a short signal (in milliseconds)	
	static const unsigned int DELAY_LONG;///< used to encode a long signal (in milliseconds)	
	static const unsigned int DELAY_LETTER;///< used to encode the end of a letter (in milliseconds)
	static const unsigned int DELAY_SPACE;///< used to encode space between words (in milliseconds)

	/** \brief Constructor

		output is a function pointer that is called, when a complete message arrived. 
			   Its parameter is this message.
		send is a function pointer that is called to send a UdpPacket via the socket.
		getPacket is a function pointer, that has to return a valid UdpPacket.
	*/
	TimingCovertChannel(function<void(string)> output, 
						function<void(UdpPacket)> send, 
						function<UdpPacket(void)> getPacket)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_getPacket(getPacket),
		m_coder(DELAY_SHORT, DELAY_LONG, DELAY_LETTER, DELAY_SPACE),
		m_receiving(0)
		{};

	/**
		Sends a message using the timing channel. The argument is the message as a string.
	*/
	void sendMessage(string message);

	/**
		Has to be called when a packet is received, with the packet as the argument. 
		After a timeout of 2 seconds the callback m_output is called with the received message.
	*/
	void receivePacket(GenericPacket& packet);

	/**
		Returns a string with the name of the covert channel "Timing Covert Channel"
	*/ 
	string name() const;

	/**
		Returns a string with basic information about the timing covert channel
	*/ 
	string info() const;

private:
	/**
		Starts the timeout. If m_timeout_changed is false and m_timeout_end is in the past, 
		it calls the callback function m_output.
	*/
	void startTimeoutTimer();

	/**
		Sends Udp packets using m_send with inter-packet delays given as the argument.
	*/
	void sendDelays(vector<unsigned int> delays);

	MorseCoder m_coder;///<MorseCoder is used to encode messages as morse	
	function<void(string)> m_output;///< callback function pointer that is used to return received messages as a string	
	function<void(UdpPacket)> m_send;///< function pointer that is used to send Udp Packets via the socket	
	function<UdpPacket(void)> m_getPacket;///< function pointer that is used to retrieve valid udp packets, that are send with delay
	chrono::high_resolution_clock::time_point m_receive_start;///< Marks the time the last packet was received to measure inter-packet delays
	chrono::high_resolution_clock::time_point m_timeout_end;///< Marks the time point at which the timeout ends. It is increased by 2 seconds each time a packet is received.
	atomic<bool> m_timeout_changed;///< signals 'startTimeoutTimer' that the timeout end point has changed.	
	vector<unsigned int> m_received_delays;///< stores the measured delays
	atomic<bool> m_receiving;///< indicates, if the channel is currently receiving a message

};
}
#endif // TIMING_COVERT_CHANNEL
