#include <whisperLibrary\channelmanager.hpp>
#include <iostream>


using namespace std;

int main(int argc, char* argv[]){
	whisper_library::ChannelManager channelmanager;
	channelmanager.selectChannel("TCP Header Covert Channel");
	cout << "current channel: " << channelmanager.currentChannel() << endl;

	string my_ip = "127.0.0.1";
	cout << "my ip:" << my_ip << endl;
	//channelmanager.openConnection(my_ip, 20);
}
