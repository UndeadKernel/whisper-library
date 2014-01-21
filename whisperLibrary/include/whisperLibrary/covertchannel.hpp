
#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include <common.hpp>
#include <channelmanager.hpp>

namespace whisper_library {

/*
	CovertChannel is the abstract class all covert channels you want use have to inherit from. It defines the
	interface for a covert channel. You can add CovertChannel objects to the ChannelManager to use them.
*/
class WHISPERAPI CovertChannel {

public:
	// constructor
	CovertChannel(ChannelManager* channelmanager) {};
	// sends a message through the covert channel
	virtual void sendMessage(std::string message) = 0;
	// receive a message
	virtual void receiveMessage(TcpPacket& packet) = 0;
private:
	

private:
};
}
#endif // COVERT_CHANNEL