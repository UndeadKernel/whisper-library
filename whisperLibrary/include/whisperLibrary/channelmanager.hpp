/*	<channelmanager.hpp>
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

#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include "common.hpp"
#include <vector>
#include "../../src/tcppacket.hpp"
#include <iostream>
#include "tcpheadercovertchannel.hpp"
#include "timingcovertchannel.hpp"
#include <functional>
#include <genericpacket.hpp>
#include "../../src/udppacket.hpp"
#include <map>
#include "../../src/networkconnector.hpp"

namespace whisper_library {

class CovertChannel;

/**
	\brief A ChannelManager is the connecting part for the covert channel and the framework. 
	
	You can add and remove covert channels you want to use. 
	You pass the data you want to send to this class.
	It is connected to the network via a SocketConnector.
	\class ChannelManager channelmanager.hpp
*/
class WHISPERAPI ChannelManager {

public:
	// constructor
	ChannelManager();
	~ChannelManager();

	// Error handling

	/**
	* \brief Sets the stream, that the covert channel uses as the output for received messages
	* \param a pointer to the output stream
	*/
	void setErrorStream(std::ostream* stream);
	void outputErrorMessage(string message);
	void setMessageCallback(function<void(string, string)> message_callback);

	// Covert channel
	vector<string> getChannelInfos();
	vector<string> getChannelNames();
	unsigned int channelCount();
	/*
	Sets the arguments of the current covert channel.
	The string arguments is parsed by the channel to set channel specific options.
	More information on which arguments are supported can be found in the specific covertchannel source.
	*/
	void setChannelArguments(string ip, string arguments);

	// Callbacks for Covert Channels

	/** 
	 * \brief creates a valid tcp packet
	 * \return a tcp packet
	 */
	TcpPacket getTcpPacket();
	UdpPacket getUdpPacket(unsigned short port);

	void setOutputStream(std::ostream* stream);
	/**
	* \brief Writes a message to the selected output stream, stored in m_output_stream.
	* \param message the message to be written
	*/
	void outputMessage(string ip, string message);

	/** 
	 * \brief Is called, when the socket receives a packet of the communication
	 * It is then forwarded to the channel with the specified ip.
	 * \param packet the packet that was received
	 */
	void packetReceived(string ip, GenericPacket packet);

	// Connection
	bool openConnection(string ip, unsigned int channel_id);
	void closeConnection(string ip);
	unsigned int connectionCount();
	/**
	* \brief Sends a message through the currently selected covert channel
	* \param the message as string
	*/
	void sendMessage(string ip, string message);

	// Adapter handling
	void setAdapter(string name);
	unsigned int adapterCount();
	vector<string> networkAdapters();
	string adapterDescription(string adapter_name);
	vector<string> adapterAddresses();
	
private:
	CovertChannel* createChannel(string ip, unsigned int channel_id);

	// hold all available channels, just to pull infos
	std::vector<CovertChannel*> m_channels;
	// stream that holds received messages
	std::ostream* m_output_stream;
	// stream that displays errors
	std::ostream* m_error_stream;

	NetworkConnector* m_network;
	map<string, CovertChannel*> m_ip_mapping;
	const unsigned int CHANNEL_COUNT;
	function<void(string, string)> m_message_callback;
};
}
#endif // CHANNEL_MANAGER
