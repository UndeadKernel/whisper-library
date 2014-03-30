#include <channelmanager.hpp>
#include "../include/whisperLibrary/whisper_library_wrapper.hpp"

whisper_library::ChannelManager* m_channel_manager = NULL;

void wlMakeChannelManager(){
	m_channel_manager = new whisper_library::ChannelManager();
}
void wlDestroyChannelManager(){

}
void wlAddBuddy(const char* who, const char* channel){
	
}
void wlSetAdapter(const char* adapter){
	
}
void wlSetOptions(const char* options){
	
}
void wlSendMessage(const char* who, const char* message){
	
}
WhisperMessage wlGetMessage(){
	WhisperMessage ret;
	ret.who = "who";
	ret.message = "message";
	return ret;
}

char* wlListAdapters(){
	return "no one";
}
char* wlListChannels(){
	return "no one";
}
char* wlSelectedAdapter(){
	return "no one";
}
char* wlSelectedChannel(){
	return "no one";
}

void wlGetMessageCallback(void (*func_ptr)(const char*, const char*)){
	m_channel_manager->setMessageCallback(func_ptr);
}
