#ifndef CHANNEL_MANAGER
#define CHANNEL_MANAGER

#include "common.hpp"
#include <vector>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisperLibrary {

class WHISPERAPI ChannelManager {

public:
	ChannelManager();
	void addChannel(CovertChannel* chan);
	void removeChannel(CovertChannel* chan);
	

private:
	std::vector<CovertChannel*> channels;
	SocketConnector socket;
};

}

#endif