#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
	using namespace std::placeholders;
	//m_socket = new SocketConnector(this);

	// TcpHeaderCovertChannel
	//std::function<void(string)> f_output = std::bind(&ChannelManager::outputMessage, this, _1);
	//std::function<void(TcpPacket)> f_send = std::bind(&ChannelManager::sendTCPPacket, this, _1);
	//std::function<TcpPacket(void)> f_packet = std::bind(&ChannelManager::getTcpPacket, this);
	//addChannel((new TcpHeaderCovertChannel(f_output, f_send, f_packet)));
	/*addChannel((new TcpHeaderCovertChannel(std::bind(&ChannelManager::outputMessage, this, _1),
										   std::bind(&ChannelManager::sendTCPPacket, this, _1), 
										   std::bind(&ChannelManager::getTcpPacket, this))));*/
	CovertChannel* cc = new TcpHeaderCovertChannel(std::bind(&ChannelManager::outputMessage, this, _1),
													std::bind(&ChannelManager::sendTCPPacket, this, _1),
													std::bind(&ChannelManager::getTcpPacket, this));
	cout << cc->test() << endl;
	delete cc;

	//m_current_channel = m_channels[0];
}
ChannelManager::~ChannelManager() {
	m_channels.clear();
	//delete m_socket;
}

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.push_back(channel);
}

//callback method for CC
void ChannelManager::outputMessage(std::string message){
	cout << "ChannelManager: message output " << message << endl;
	//cout << message << endl;
}

void ChannelManager::sendTCPPacket(TcpPacket packet) {
	m_socket->sendPacket(packet);
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