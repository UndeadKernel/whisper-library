#include "whisperLibrary/channelmanager.hpp"
#include "whisperLibrary/morseCoder.hpp"
#include <iostream>


using namespace std;
typedef bitset<6> bitset6;

int main(int argc, char* argv[]){
/*	whisper_library::ChannelManager channelmanager;
	vector<string> channels = channelmanager.getChannelNames();
	vector<string> channel_infos = channelmanager.getChannelInfos();

	for (unsigned int i = 0; i < channels.size(); i++) {
		cout << "[" << i << "] " << channels[i] << ": " << channel_infos[i] << endl;
	} */

	whisper_library::MorseCoder mc;
	vector<int> encoding = mc.encodeLetter('A');

	for (vector<int>::iterator it = encoding.begin(); it != encoding.end(); i++) {
		cout << (*it);
	}
	cout << endl;
}
