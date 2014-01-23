// Author: Martin Oehler

#ifndef TCP_HEADER_COVERT_CHANNEL
#define TCP_HEADER_COVERT_CHANNEL

#include "common.hpp"
#include "tcppacket.hpp"
#include <string>
#include <vector>
#include <bitset>
#include "bitSetCoder.hpp"
#include "covertchannel.hpp"
#include <functional>

using namespace std;

namespace whisper_library {

	/*
		Covert Channel, that uses 3 bits of the TCP Header, that are not used, to hide a message.
		To use this class, call 'sendMessage' with your message as a string. Received packets are used to class 'receiveMessage'.
		The decoded message is given to the callback function 'm_output'.
	*/
	class TcpHeaderCovertChannel : public CovertChannel {

	public:
		/*
			Constructor
			output is a function pointer that is called, when a complete message arrived. Its parameter is this message.
			send is a function pointer that is called to send a TcpPacket via the socket.
			getPacket is a function pointer, that has to return a valid TcpPacket, that is used to insert the data.
		*/
		TcpHeaderCovertChannel(function<void(string)> output, function<void(TcpPacket)> send, function<TcpPacket(void)> getPacket)
			: CovertChannel(),
				m_numb_packets(0), 
				m_output(output), 
				m_send(send),
				m_getPacket(getPacket) {};
		/*
			SendMessage sends a message of type string through the Tcp Header Covert Channel.
		*/
		void sendMessage(string message);
		/*
			This function is called, when a new packet arrives at the socket. It collects them and returns the message via the callback function "m_output".
		*/
		void receiveMessage(TcpPacket& packet);

	private:
		/*
			encodeMessageWithLength splits the message into parts of 3 bits and adds length blocks inbetween.
		*/
		vector<bitset<3>> encodeMessageWithLength(string message);
		/*
			modifyTcpPacket sets the reserved bits of packet to the value of data.
		*/
		void modifyTcpPacket(TcpPacket& packet, bitset<3> data);
		/*
			extractData extracts the data we hid in packet.
		*/
		bitset<3> extractData(TcpPacket& packet);
		vector<bitset<3>> m_data_blocks;
		int m_numb_packets;
		BitSetCoder<3> m_coder;
		function<void(string)> m_output;
		function<void(TcpPacket)> m_send;
		function<TcpPacket(void)> m_getPacket;

	};

}

#endif // TCP_HEADER_COVERT_CHANNEL