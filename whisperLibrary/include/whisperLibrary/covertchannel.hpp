
#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include "common.hpp"
#include "genericpacket.hpp"


namespace whisper_library {

/*
	CovertChannel is the abstract class all covert channels you want to use have to inherit from. It defines the
	interface for a covert channel. You can add CovertChannel objects to the ChannelManager to use them.
*/
class CovertChannel {
public:
	// Empty constructor
	CovertChannel() {};
	// Virtual destructor so that the destructor of derived classes is called
	virtual ~CovertChannel() {};
	// Call this function to send a message using the covert channel.
	virtual void sendMessage(std::string message) = 0;
	// This function is called, when a new packet arrived. The argument is a reference to this packet.
	virtual void receiveMessage(GenericPacket& packet) = 0;
	// Displays the name of the covert channel
	virtual string name() const = 0;
	// Displays information about the covert channel
	virtual string info() const = 0;
	// Returns the used protocol (e.g. tcp)
	virtual string protocol() const = 0;
};
}
#endif // COVERT_CHANNEL
