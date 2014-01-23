#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
	m_socket = new SocketConnector(this);
	m_output_message = "";

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
	cout << "ChannelManager: message output " << message << endl;
	m_output_message += message;
	
}

string ChannelManager::getOutputMessage() {
	return m_output_message;
}

void ChannelManager::sendMessage(string message) {
	if (m_current_channel != NULL) {
		cout << "ChannelManager: sending message " << message << endl;
		m_current_channel->sendMessage(message);
	}
}


TcpPacket ChannelManager::getTcpPacket(){
	return TcpPacket();
}

void ChannelManager::packetReceived(TcpPacket packet) {
	if (m_current_channel != NULL) {
		cout << "ChannelManager: packet received" << endl;
		m_current_channel->receiveMessage(packet);
	}
}

void ChannelManager::selectChannel(unsigned int index) {
	if (index >= 0 && index < m_channels.size()) {
		m_current_channel = m_channels[index];
	}
}

}