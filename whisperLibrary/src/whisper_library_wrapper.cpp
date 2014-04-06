#include <channelmanager.hpp>
#include "../include/whisperLibrary/whisper_library_wrapper.hpp"

whisper_library::ChannelManager* m_channel_manager = NULL;

void wlMakeChannelManager(){
	m_channel_manager = new whisper_library::ChannelManager();
}
void wlDestroyChannelManager(){
	delete m_channel_manager;
}

ChannelList* wlListChannels(){
	vector<string> ids = m_channel_manager->getChannelIDs();
	vector<string> names = m_channel_manager->getChannelNames();
	vector<string> infos = m_channel_manager->getChannelInfos();
	
	ChannelList* first = new ChannelList;
	first->id = new char[ids[0].length() + 1];
	std::strcpy(first->id, ids[0].c_str());

	first->name = new char[names[0].length() + 1];
	std::strcpy(first->name, names[0].c_str());

	first->info = new char[infos[0].length() + 1];
	std::strcpy(first->info, infos[0].c_str());

	ChannelList** current = &(first->next);
	for (unsigned int i = 1; i < ids.size(); i++) {
		(*current) = new ChannelList;
		(*current)->next = NULL;

		(*current)->id = new char[ids[i].length() + 1];
		std::strcpy((*current)->id, ids[i].c_str());

		(*current)->name = new char[names[i].length() + 1];
		std::strcpy((*current)->name, names[0].c_str());

		(*current)->info = new char[infos[i].length() + 1];
		std::strcpy((*current)->info, infos[i].c_str());

		current = &((*current)->next);
	}

	return first;
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

AdapterList* wlListAdapters(){
	vector<string> adapters = m_channel_manager->networkAdapters();
	AdapterList* first = new AdapterList;
	first->name = new char[adapters[0].length() + 1];
	std::strcpy(first->name, adapters[0].c_str());

	string description = m_channel_manager->adapterDescription(adapters[0]);
	first->description = new char[description.length() + 1];
	std::strcpy(first->description, description.c_str());

	AdapterList** current = &(first->next);
	for (unsigned int i = 1; i < adapters.size(); i++) {
		(*current) = new AdapterList;
		(*current)->next = NULL;

		(*current)->name = new char[adapters[i].length() + 1];
		std::strcpy((*current)->name, adapters[i].c_str());

		description = m_channel_manager->adapterDescription(adapters[i]);
		(*current)->description = new char[description.length() + 1];
		std::strcpy((*current)->description, description.c_str());

		current = &((*current)->next);
	}

	return first;
}

void (*m_callback)(const char*, const char*);

void messageCallback(string ip, string message) {
	m_callback(ip.c_str(), message.c_str());
}

void wlSetMessageCallback(void(*func_ptr)(const char*, const char*)) {
	m_callback = func_ptr;
	m_channel_manager->setMessageCallback(&messageCallback);
}
