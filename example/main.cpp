<<<<<<< HEAD
#include "whisperLibrary\channelmanager.hpp"
=======
#include <whisperLibrary/socketconnector.hpp>
#include <whisperLibrary/bitSetEncoder.hpp>
#include <whisperLibrary/bitSetDecoder.hpp>
>>>>>>> master

#include <iostream>


using namespace std;
typedef bitset<6> bitset6;

int main(int argc, char* argv[]){
	whisper_library::ChannelManager channelmanager;
	vector<string> channels = channelmanager.getChannelNames();
	vector<string> channel_infos = channelmanager.getChannelInfos();
}
