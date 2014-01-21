#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
}

void ChannelManager::outputMessage(std::string message){
}

TcpPacket ChannelManager::getTcpPacket(){
	return TcpPacket();
}

void ChannelManager::packetReceived(TcpPacket packet) {
	m_current_channel->receiveMessage(packet);
}

void ChannelManager::selectChannel(int index) {
	if (index > 0 && index < m_channels.size()) {
		m_current_channel = m_channels[index];
	}
}

SocketConnector* ChannelManager::sender() {
	return m_socket;
}
}