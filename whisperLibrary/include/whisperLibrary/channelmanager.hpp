#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include "common.hpp"
#include <vector>
#include "../../src/tcppacket.hpp"
#include <iostream>
#include "tcpheadercovertchannel.hpp"
#include <functional>
#include <regex>

namespace whisper_library {

class CovertChannel;
class SocketConnector;

/*
	A ChannelManager is the connecting part for the covert channel and the framework. You can add and remove
	covert channels you want to use. You pass the data you want to send to this class.

	It is connected to the network via a SocketConnector.
*/
class WHISPERAPI ChannelManager {

public:
	// constructor
	ChannelManager();
	~ChannelManager();
	// Selects the channel for communication at 'index' of m_channels.
	void selectChannel(unsigned int index);
	void selectChannel(string name);
	// Sends a message through the currently selected covert channel
	void sendMessage(string message);
	// Sets the stream, that the covert channel uses as the output for received messages
	void setOutputStream(std::ostream* stream);
	vector<string> getChannelInfos();
	vector<string> getChannelNames();
	// returns the name of the currently selected channel
	string currentChannel();
	void openConnection(string ip, short port);
	// Writes 'message' to the selected output stream, stored in m_output_stream.
	void outputMessage(std::string message);
	// creates a valid tcp packet
	TcpPacket getTcpPacket();
	// Is called, when the socket receives a tcp packet of the communication
	void packetReceived(TcpPacket packet);
	
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