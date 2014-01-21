#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include "common.hpp"
#include <vector>
#include "tcppacket.hpp"
#include <iostream>
#include "tcpheadercovertchannel.hpp"
#include <functional>

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
	void selectChannel(int index);
	void outputMessage(std::string message);
	void sendTCPPacket(TcpPacket packet);
	TcpPacket getTcpPacket();
	void packetReceived(TcpPacket packet);
	
private:
	// adds a channel to the available channels
	void addChannel(CovertChannel* channel);
	//removes a channel from the available channels
	void removeChannel(CovertChannel* channel);
	// hold all available channels
	std::vector<CovertChannel*> m_channels;
	CovertChannel* m_current_channel;
	// connects the ChannelManager to the network
	SocketConnector* m_socket;
};
}
#endif // CHANNEL_MANAGER