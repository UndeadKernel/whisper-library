#include "whisperLibrary/channelmanager.hpp"
#include <iostream>


using namespace std;
typedef bitset<6> bitset6;

int main(int argc, char* argv[]){
	whisper_library::ChannelManager channelmanager;
	vector<string> channels = channelmanager.getChannelNames();
	vector<string> channel_infos = channelmanager.getChannelInfos();

	for (unsigned int i = 0; i < channels.size(); i++) {
		cout << "[" << i << "] " << channels[i] << ": " << channel_infos[i] << endl;
	}

	channelmanager.selectChannel("TCP Header Covert Channel");
	cout << "current channel: " << channelmanager.currentChannel() << endl;

	string my_ip = "127.0.0.1";
	cout << "my ip:" << my_ip << endl;
	channelmanager.openConnection(my_ip, 20);
}
