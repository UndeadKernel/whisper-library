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
#include <udppacket.hpp>

namespace whisper_library {

class CovertChannel;
class SocketConnector;

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
	/** 
	 * \brief Selects the channel for communication using the 'index' of m_channels.
	 * \param index the index of the channel
	 */
	void selectChannel(unsigned int index);
	/** 
	 * \brief Writes a message to the selected output stream, stored in m_output_stream.
	 * \param message the message to be written
	 */
	void outputMessage(std::string message);
	/** 
	 * \brief creates a valid tcp packet
	 * \return a tcp packet
	 */
	TcpPacket getTcpPacket();
	UdpPacket getUdpPacket();

	/** 
	 * \brief Is called, when the socket receives a packet of the communication
	 * \param packet the  packet that was received
	 */
	void packetReceived(GenericPacket packet);
	/** 
	 * \brief Sends a message through the currently selected covert channel
	 * \param the message as string
	 */
	void sendMessage(string message);
	/** 
	 * \brief Sets the stream, that the covert channel uses as the output for received messages
	 * \param a pointer to the output stream
	 */
	void setOutputStream(std::ostream* stream);
	vector<string> getChannelInfos();
	vector<string> getChannelNames();
	
private:
	// adds a channel to the available channels
	void addChannel(CovertChannel* channel);
	//removes a channel from the available channels
	void removeChannel(CovertChannel* channel);
	// hold all available channels
	std::vector<CovertChannel*> m_channels;
	// pointer to the covert channel, that is currently in use
	CovertChannel* m_current_channel;
	// connects the ChannelManager to the network
	SocketConnector* m_socket;
	// stream that holds received messages
	std::ostream* m_output_stream;
};
}
#endif // CHANNEL_MANAGER
