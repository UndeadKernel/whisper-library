#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <socketconnector.hpp>

namespace whisper_library {

ChannelManager::ChannelManager(){
	m_socket = new SocketConnector(this);
	m_network_sniffer = new Sniffer();

	// TcpHeaderCovertChannel
	addChannel((new TcpHeaderCovertChannel(std::bind(&ChannelManager::outputMessage, this, std::placeholders::_1),
										   std::bind(&SocketConnector::sendTcpPacket, m_socket, std::placeholders::_1),
										   std::bind(&ChannelManager::getTcpPacket, this))));
	// TimingChannel
	addChannel(new TimingCovertChannel(std::bind(&ChannelManager::outputMessage, this, std::placeholders::_1),
									   std::bind(&SocketConnector::sendUdpPacket, m_socket, std::placeholders::_1),
									   std::bind(&ChannelManager::getUdpPacket, this)));
	m_current_channel = m_channels[0];
}
ChannelManager::~ChannelManager() {
	for (unsigned int i = 0; i < m_channels.size(); i++) {
		delete m_channels[i];
	}
	delete m_socket;
	delete m_network_sniffer;
}

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.push_back(channel);
}

//callback method for CC
void ChannelManager::outputMessage(std::string message){
	(*m_output_stream) << message;
}

void ChannelManager::sendMessage(string message) {
	if (m_current_channel != NULL) {
		m_current_channel->sendMessage(message);
	}
}


TcpPacket ChannelManager::getTcpPacket(){
	return TcpPacket();
}

UdpPacket ChannelManager::getUdpPacket() {
	return UdpPacket();
}

void ChannelManager::packetReceived(GenericPacket packet) {
	if (m_current_channel != NULL) {
		m_current_channel->receiveMessage(packet);
	}
}

void ChannelManager::selectChannel(unsigned int index) {
	if (index >= 0 && index < m_channels.size()) {
		m_current_channel = m_channels[index];
	}
}

void ChannelManager::selectChannel(string name) {
	for (vector<CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); it++) {
		if ((*it)->name().compare(name) == 0) {
			m_current_channel = (*it);
		}
	}
}

void ChannelManager::setOutputStream(std::ostream* stream) {
	m_output_stream = stream;
}

void ChannelManager::setErrorStream(std::ostream* stream) {
	m_error_stream = stream;
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

string ChannelManager::currentChannel() {
	if (m_current_channel != NULL) {
		return m_current_channel->name();
	}
	else {
		return "null";
	}
}

void ChannelManager::openConnection(string ip, short port, string adapter_name) {
	selectAdapter(adapter_name);
	bool ip_good = std::regex_match(ip, std::regex("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$"));
	if (ip_good) {
		m_network_sniffer->openAdapter(m_current_adapter_id, m_network_sniffer->DEFAULT_MAXPACKETSIZE, m_network_sniffer->PROMISCUOUS_MODE_OFF);
		string filter = "host " + ip + " and port " + to_string(port) + " and " + m_current_channel->protocol();
		m_network_sniffer->applyFilter(m_current_adapter_id, filter.c_str());
		std::thread packet_receiver(std::bind(&ChannelManager::retrievePacket, this));
		packet_receiver.detach();
	}
}

void ChannelManager::retrievePacket() {
	while (true) { //TODO
		vector<bool> packet_data = m_network_sniffer->retrievePacketAsVector(m_current_adapter_id);
		if (!packet_data.empty()) {
			GenericPacket generic_packet(packet_data);
			m_current_channel->receiveMessage(generic_packet);
		}
	}
}

int ChannelManager::adapterCount() {
	return m_network_sniffer->adapterCount();
}

vector<char*> ChannelManager::adapterNames() {
	return m_network_sniffer->adapterNames();
}

vector<char*> ChannelManager::adapterDescriptions() {
	vector<char*> adapter_names;
	int count = m_network_sniffer->adapterCount();
	for (int i = 0; i < count; i++) {
		const char* description = m_network_sniffer->adapterDescription(i);
		adapter_names.push_back(const_cast<char*>(description));
	}

	return adapter_names;
}

void ChannelManager::selectAdapter(string adapter_name) {
	int adapter_id = m_network_sniffer->adapterId(adapter_name.c_str(), m_network_sniffer->ADAPTER_NAME);
	if (adapter_id < 0) {
		(*m_error_stream) << "Adapter \'" + adapter_name + "\' not found.";
	}
	else {
		m_current_adapter_id = adapter_id;
	}
}

}