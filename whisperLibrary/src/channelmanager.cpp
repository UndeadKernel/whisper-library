/*	<channelmanager.cpp>
	Copyright(C) 2013,2014  Jan Simon Bunten
							Simon Kadel
							Martin Sven Oehler
							Arne Sven Stühlmeyer

	This File is part of the WhisperLibrary

	WhisperLibrary is free software : you can redistribute it and / or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	WhisperLibrary is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <channelmanager.hpp>
#include <covertchannel.hpp>
#include <iostream>

using namespace std;

namespace whisper_library {

ChannelManager::ChannelManager(){
	m_network_sniffer = new Sniffer();
	m_socket_sender = new SocketSender();

	// TcpHeaderCovertChannel
	addChannel((new TcpHeaderCovertChannel(std::bind(&ChannelManager::outputMessage, this, std::placeholders::_1),
										   std::bind(&SocketSender::sendTcp, m_socket_sender, std::placeholders::_1),
										   std::bind(&ChannelManager::getTcpPacket, this))));
	// TimingChannel
	addChannel(new TimingCovertChannel(std::bind(&ChannelManager::outputMessage, this, std::placeholders::_1),
									   std::bind(&SocketSender::sendUdp, m_socket_sender, std::placeholders::_1),
									   std::bind(&ChannelManager::getUdpPacket, this)));
	m_current_channel = m_channels[0];
	m_current_adapter_id = -1;
	m_connected = false;
}
ChannelManager::~ChannelManager() {
	for (unsigned int i = 0; i < m_channels.size(); i++) {
		delete m_channels[i];
	}
	delete m_network_sniffer;
	delete m_socket_sender;
}

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.push_back(channel);
}

//callback method for CC
void ChannelManager::outputMessage(std::string message){
	if (m_output_stream != NULL) {
		(*m_output_stream) << message << endl;
	}
}

void ChannelManager::outputErrorMessage(string message) {
	if (m_error_stream != NULL) {
		(*m_error_stream) << message << endl;
	}
}

void ChannelManager::sendMessage(string message) {
	if (m_current_channel != NULL) {
		m_current_channel->sendMessage(message);
	}
}


TcpPacket ChannelManager::getTcpPacket(){
	// TODO
	uint sourcePort = 8080;
	uint destPort = 8080;
	ulong sequenceNumber = 1;
	bitset<4> dataOffset("1010");
	ulong ackNumber = 0;
	uint windowSize = 2;
	vector<bool> options;
	whisper_library::TcpPacket packet(sourcePort,
		destPort,
		sequenceNumber,
		ackNumber,
		dataOffset,
		windowSize,
		options);
	packet.setAcknowledgementFlag(0);
	packet.setSynchronisationFlag(1);
	return packet;
}

UdpPacket ChannelManager::getUdpPacket() {
	whisper_library::UdpPacket packet;
	packet.setSourcePort(m_current_channel->port());
	packet.setDestinationPort(m_current_channel->port());
	packet.setLength(11);
	packet.setChecksum(0);
	std::vector<char> data;
	data.push_back('A');
	data.push_back('B');
	data.push_back('C');
	packet.setData(data);
	return packet;
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

bool ChannelManager::openConnection(string ip, string adapter_name) {
	if (m_connected) {
		outputErrorMessage("Already connected. Please close the connection first.");
		return false;
	}
	selectAdapter(adapter_name);
	if (m_current_adapter_id == -1) {
		outputErrorMessage("Adapter '" + adapter_name + "' not found.");
		return false;
	}
	boost::system::error_code ec;
	boost::asio::ip::address::from_string( ip, ec);
	if (ec) {
		outputErrorMessage("Invalid IP");
		return false;
	}
	m_connected = true;
	m_socket_sender->setReceiverIp(ip);
	m_network_sniffer->openAdapter(m_current_adapter_id, m_network_sniffer->DEFAULT_MAXPACKETSIZE, true, 1);
	string filter = "src " + ip +" and port " + to_string(m_current_channel->port()) + " and " + m_current_channel->protocol();
	m_network_sniffer->applyFilter(m_current_adapter_id, filter.c_str());
	std::thread packet_receiver(std::bind(&ChannelManager::retrievePacket, this));
	packet_receiver.detach();
	return true;
}

void ChannelManager::closeConnection() {
	if (m_connected) {
		m_network_sniffer->closeAdapter(m_current_adapter_id);
		m_connected = false;
	}
}

void ChannelManager::retrievePacket() {
	vector<bool> packet_data;
	GenericPacket generic_packet;
	while (m_connected) {
		packet_data = m_network_sniffer->retrievePacketAsVector(m_current_adapter_id);
		if (!packet_data.empty()) {
			generic_packet.setContent(packet_data);
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

const char* ChannelManager::adapterDescription(string adapter_name) {
	int adapter_id = m_network_sniffer->adapterId(adapter_name.c_str(), m_network_sniffer->ADAPTER_NAME);
	if (adapter_id == -2) {
		outputErrorMessage("Adapter '" + adapter_name + "' not found.");
		return "";
	}
	return m_network_sniffer->adapterDescription(adapter_id);
}

void ChannelManager::selectAdapter(string adapter_name) {
	cout << "received adapter name: " << adapter_name << endl;
	int adapter_id = m_network_sniffer->adapterId(adapter_name.c_str(), m_network_sniffer->ADAPTER_NAME);
	cout << "selected adapter id: " << adapter_id << endl;
	if (adapter_id < 0) {
		adapter_id = -1;	//adapter not found
	}
	else {
		m_current_adapter_id = adapter_id;
	}
}

bool ChannelManager::connected() {
	return m_connected;
}

}
