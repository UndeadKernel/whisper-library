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
#include <string>
#include "../../src/udppacket.hpp"
#include <mutex>

using namespace std;

namespace whisper_library {
	/*
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
		Constructor
		output is a function pointer that is called, when a complete message arrived. 
			   Its parameter is this message.
		send is a function pointer that is called to send a UdpPacket via the socket.
		getPacket is a function pointer, that has to return a valid UdpPacket.
	*/
	TimingCovertChannel(function<void(string)> output, 
						function<void(UdpPacket)> send, 
						function<UdpPacket(unsigned short)> getPacket)
		: CovertChannel(),
		m_output(output),
		m_send(send),
		m_getPacket(getPacket),
		m_receiving(false)
	{
		m_delay_short = 100;
		m_delay_long = 300;
		m_delay_letter = 500;
		m_delay_space = 700;
		calculateTresholds();
		m_coder = new MorseCoder(m_delay_short, m_delay_long, m_delay_letter, m_delay_space);
	};

	// Destructor
	~TimingCovertChannel(){
		delete m_coder;
	}
	/*
		Sends a message using the timing channel. The argument is the message as a string.
	*/
	void sendMessage(string message);

	/*
		Has to be called when a packet is received, with the packet as the argument. 
		After a timeout of 2 seconds the callback m_output is called with the received message.
	*/
	void receiveMessage(GenericPacket& packet);

	/** \brief Gives arguments to the channel to configure the delays

		Available arguments: -set_timings [short] [long] [letter] [space]
		[short] [long] [letter] [space] are the delays used for morse encoding in milliseconds
		\param arguments string that is parsed for channel arguments
	*/
	void setArguments(string arguments);

	// Returns a string with the name of the covert channel "Timing Covert Channel"
	string name() const;

	// Returns a string with basic information about the timing covert channel
	string info() const;

	// Returns the protocol used by this covert channel (udp)
	string protocol() const;

	// Returns the used port (23)
	short port() const;
private:
	void calculateTresholds();
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
	MorseCoder* m_coder;

	// callback function pointer that is used to return received messages as a string
	function<void(string)> m_output;

	// function pointer that is used to send Udp Packets via the socket
	function<void(UdpPacket)> m_send;

	// function pointer that is used to retrieve valid udp packets, that are send with delay
	function<UdpPacket(unsigned short)> m_getPacket;

	// Marks the time the last packet was received to measure inter-packet delays
	chrono::time_point<chrono::system_clock> m_receive_start;

	/*
		Marks the time point at which the timeout ends. 
		It is increased by 2 seconds each time a packet is received.
	*/
	chrono::time_point<chrono::system_clock> m_timeout_end;

	// signals 'startTimeoutTimer' that the timeout end point has changed.
	atomic<bool> m_timeout_changed;

	// stores the measured delays
	vector<unsigned int> m_received_delays;

	// indicates, if the channel is currently receiving a message
	atomic<bool> m_receiving;

	// timeout in milliseconds after the last received message until the delays are interpreted
	unsigned int m_timeout;

	/*
		TODO: calculate delays based on connection
	*/
	// delay_short is used to encode a short signal (in milliseconds)
	unsigned int m_delay_short;
	unsigned int m_threshold_delay_short;

	// delay_long is used to encode a long signal (in milliseconds)
	unsigned int m_delay_long;
	unsigned int m_threshold_delay_long;

	// delay_letter is used to encode the end of a letter (in milliseconds)
	unsigned int m_delay_letter;
	unsigned int m_threshold_delay_letter;

	// delay_space is used to encode space between words (in milliseconds)
	unsigned int m_delay_space;

	mutex m_mutex_sending; ///< mutex to prevent different threads from sending simultaneously
};
}
#endif // TIMING_COVERT_CHANNEL
