//author: Simon Kadel

#include <whisperLibrary/socketconnector.hpp>
#include <iostream>


int main(int argc, char* argv[]){
	whisperLibrary::SocketConnector sc = whisperLibrary::SocketConnector();
	std::cout << sc.getPort() << std::endl;
}