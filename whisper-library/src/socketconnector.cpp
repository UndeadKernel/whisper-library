//author: Simon Kadel

#include <socketconnector.hpp>

namespace whisperLibrary{

SocketConnector::SocketConnector(){
	port = 42;
}

int SocketConnector::getPort(){
	return port;
}
}