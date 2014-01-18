#ifndef SOCKET_CONNECTOR
#define SOCKET_CONNECTOR

#include "common.hpp"

namespace whisper_library {

class SocketConnector {

public:
	SocketConnector();
	void openConnection(int protokoll);
	//void send(TcpPackage package);
	//void send(UdpPackage package);
};
}
#endif // SOCKET_CONNECTOR