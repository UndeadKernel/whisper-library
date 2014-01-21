#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
	m_socket = new SocketConnector(this);

	std::function<void(string)> f_output = std::bind(&outputMessage, this);
	std::function<void(TcpPacket)> f_send = &sendTCPPacket;
	std::function<TcpPacket(void)> f_packet = &getTcpPacket;
	CovertChannel* tcpCC = new TcpHeaderCovertChannel(f_output, f_send, f_packet);
	addChannel(tcpCC);
}
ChannelManager::~ChannelManager() {
	for (int i = 0; i < m_channels.size(); i++) {
		delete m_channels[i];
	}
	m_channels.clear();
}

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.push_back(channel);
}

//callback method for CC
void ChannelManager::outputMessage(std::string message){
	cout << message << endl;
}

void ChannelManager::sendTCPPacket(TcpPacket packet) {
	m_socket->sendPacket(packet);
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

}