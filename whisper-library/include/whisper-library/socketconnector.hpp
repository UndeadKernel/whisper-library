//author: Simon Kadel

#ifndef SOCKET_CONNECTOR
#define SOCKET_CONNECTOR


#include "common.hpp"
#include <boost/asio.hpp>

namespace whisperLibrary {

class WHISPERAPI SocketConnector {

public:
	SocketConnector();
	int getPort();
	
private:
	int port;

};

}

#endif