#include <channelmanager.hpp>
#include "../include/whisperLibrary/whisper_library_wrapper.hpp"

whisper_library::ChannelManager* m_channel_manager = NULL;

void wlMakeChannelManager(){
	m_channel_manager = new whisper_library::ChannelManager();
}
void wlDestroyChannelManager(){
	delete m_channel_manager;
}

const char* wlListChannels(){
	vector<string> ids = m_channel_manager->getChannelIDs();
	vector<string> names = m_channel_manager->getChannelNames();
	vector<string> info = m_channel_manager->getChannelInfos();
	string channel_text = "";
	for (unsigned int i = 0; i < names.size(); i++) {
		channel_text += "[" + ids[i] + "]" + names[i] + ": " + info[i] + "\n\n";
	}
	char* cstr = new char[channel_text.length() + 1];
	std::strcpy(cstr, channel_text.c_str());
	return cstr;
}

unsigned int wChannelCount() {
	return m_channel_manager->channelCount();
}

void wlSetOptions(const char* ip, const char* options) {
	m_channel_manager->setChannelArguments(ip, options);
}

bool wlOpenConnection(const char* ip, const char* channel_id) {
	return m_channel_manager->openConnection(ip, channel_id);
}

void wlCloseConnection(const char* ip) {
	m_channel_manager->closeConnection(ip);
}

unsigned int wlConnectionCount() {
	return m_channel_manager->connectionCount();
}

void wlSendMessage(const char* ip, const char* message){
	m_channel_manager->sendMessage(ip, message);
}

bool wlSetAdapter(const char* adapter){
	return m_channel_manager->setAdapter(adapter);
}

unsigned int wlAdapterCount() {
	return m_channel_manager->adapterCount();
}

const char* wlListAdapters(){
	vector<string> adapters = m_channel_manager->networkAdapters();
	string adapter_text = "";
	for (unsigned int i = 0; i < adapters.size(); i++) {
		adapter_text += "[" + to_string(i) + "] " + adapters[i] + ": " + m_channel_manager->adapterDescription(adapters[i]) + "\n";
	}
	char* cstr = new char[adapter_text.length() + 1];
	std::strcpy(cstr, adapter_text.c_str());
	return cstr;
}

void (*m_callback)(const char*, const char*);

void messageCallback(string ip, string message) {
	m_callback(ip.c_str(), message.c_str());
}

void wlSetMessageCallback(void(*func_ptr)(const char*, const char*)) {
	m_callback = func_ptr;
	m_channel_manager->setMessageCallback(&messageCallback);
}