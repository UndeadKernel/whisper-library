//author: Simon Kadel

#include <socketconnector.hpp>

#include <boost\asio.hpp>

namespace whisperLibrary{

SocketConnector::SocketConnector(){
	port = 42;
}

int SocketConnector::getPort(){
	return port;
}
}