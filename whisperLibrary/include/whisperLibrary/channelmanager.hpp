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
#include <functional>
#include <genericpacket.hpp>
#include "../../src/udppacket.hpp"

namespace whisper_library {

class CovertChannel;
class SocketConnector;

/**
	\brief The connecting part for the CovertChannel and the framework. 
	
	You can add and remove covert channels you want to use. 
	You pass the data you want to send to this class.
	It is connected to the network via a SocketConnector.
	
*/
class WHISPERAPI ChannelManager {

public:
	/** \brief Creates a ChannelManager
	*/
	ChannelManager();
	/** \brief Deletes this ChannelManager
	*/
	~ChannelManager();
	/** \brief Selects the channel for communication using the 'index' of m_channels.
		\param index the index of the channel
	 */
	void selectChannel(unsigned int index);
	/** \brief Writes a message to the selected output stream, stored in m_output_stream.
		\param message the message to be written
	 */
	void outputMessage(std::string message);
	/** \brief creates a valid tcp packet
	 */
	TcpPacket getTcpPacket();
	/** \brief creates a valid udp packet
	*/
	UdpPacket getUdpPacket();
	/** \brief creates a valid udp packet with given length
		\param length the desired length
	*/
	UdpPacket getUdpPacketWithLength(int length);

	/** \brief Is called, when the socket receives a packet of the communication
		\param packet the  packet that was received
	 */
	void packetReceived(GenericPacket packet);
	/** \brief Sends a message through the currently selected covert channel
		\param message the message as string
	 */
	void sendMessage(string message);
	/** \brief Sets the stream, that the covert channel uses as the output for received messages
		\param stream a pointer to the output stream
	 */
	void setOutputStream(std::ostream* stream);
	/**
		\return information about the available covert channels
	*/
	vector<string> getChannelInfos();
	/** \return a list of available covert channels
	*/
	vector<string> getChannelNames();
	
private:
	/** \brief adds a channel to the available channels
	*/ 
	void addChannel(CovertChannel* channel);
	/** \brief removes a channel from the available channels
	*/
	void removeChannel(CovertChannel* channel);	
	std::vector<CovertChannel*> m_channels;///< holds all available channels	
	CovertChannel* m_current_channel;///< pointer to the covert channel, that is currently in use
	SocketConnector* m_socket;///< connects the ChannelManager to the network
	std::ostream* m_output_stream;///< stream that holds received messages
};
}
#endif // CHANNEL_MANAGER
