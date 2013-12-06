//author: Simon Kadel

#ifndef SOCKET_CONNECTOR
#define SOCKET_CONNECTOR


#include "common.hpp"

namespace whisperLibrary {

class WHISPERAPI SocketConnector {

public:
	SocketConnector();
	void openConnection(int protokoll);
	//void send(TcpPackage package);
	//void send(UdpPackage package);

	
private:
	int port;

};

}

#endif