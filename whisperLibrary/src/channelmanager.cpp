#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
	m_socket = new SocketConnector(this);

	using namespace std::placeholders;
	// TcpHeaderCovertChannel
	addChannel((new TcpHeaderCovertChannel(std::bind(&ChannelManager::outputMessage, this, _1),
										   std::bind(&SocketConnector::sendPacket, m_socket, _1), 
										   std::bind(&ChannelManager::getTcpPacket, this))));
	m_current_channel = m_channels[0];
}
ChannelManager::~ChannelManager() {
	for (unsigned int i = 0; i < m_channels.size(); i++) {
		delete m_channels[i];
	}
	delete m_socket;
}

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.push_back(channel);
}

//callback method for CC
void ChannelManager::outputMessage(std::string message){
	cout << "ChannelManager: message output '" << message << "'" << endl;
	(*m_output_stream) << message;
}

void ChannelManager::sendMessage(string message) {
	if (m_current_channel != NULL) {
		cout << "ChannelManager: sending message '" << message << "'" << endl;
		m_current_channel->sendMessage(message);
	}
}


TcpPacket ChannelManager::getTcpPacket(){
	return TcpPacket();
}

void ChannelManager::packetReceived(TcpPacket packet) {
	if (m_current_channel != NULL) {
		m_current_channel->receiveMessage(packet);
	}
}

void ChannelManager::selectChannel(unsigned int index) {
	if (index >= 0 && index < m_channels.size()) {
		m_current_channel = m_channels[index];
	}
}

void ChannelManager::setOutputStream(std::ostream* stream) {
	m_output_stream = stream;
}

vector<string> ChannelManager::getChannelInfos() {
	vector<string> string_vector;
	for (vector<CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		string_vector.push_back((*it)->info());
	}
	return string_vector;
}

vector<string> ChannelManager::getChannelNames() {
	vector<string> string_vector;
	for (vector<CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		string_vector.push_back((*it)->name());
	}
	return string_vector;
}

}