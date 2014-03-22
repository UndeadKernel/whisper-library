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

ChannelManager::ChannelManager():CHANNEL_COUNT(2){
	m_network = new NetworkConnector(bind(&ChannelManager::packetReceived, this, placeholders::_1, placeholders::_2));
	// create a list of all available channels to display names and descriptions
	for (unsigned int i = 0; i < CHANNEL_COUNT; i++) {
		m_channels.push_back(createChannel("", i));
	}
}
ChannelManager::~ChannelManager() {
	// clean up open connections
	for (map<string, CovertChannel*>::iterator it = m_ip_mapping.begin(); it != m_ip_mapping.end(); it++) {
		pair<string, CovertChannel*> element = *it;
		delete element.second;
	}
	// clean up m_channels
	for (unsigned int i = 0; i < m_channels.size(); i++) {
		delete m_channels[i];
	}

	delete m_network;
}


void ChannelManager::setErrorStream(std::ostream* stream) {
	m_error_stream = stream;
}

void ChannelManager::outputErrorMessage(string message) {
	if (m_error_stream != NULL) {
		(*m_error_stream) << "Error: " << message << endl;
	}
}

// Covert Channels

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

unsigned int ChannelManager::channelCount() {
	return CHANNEL_COUNT;
}

void ChannelManager::sendMessage(string ip, string message) {
	CovertChannel* channel;
	try {
		 channel = m_ip_mapping.at(ip);
	}
	catch (out_of_range e) {
		outputErrorMessage("No connection to " + ip);
		return;
	}
	channel->sendMessage(message);
}

void ChannelManager::setChannelArguments(string ip, string arguments) {
	CovertChannel* channel;
	try {
		channel = m_ip_mapping.at(ip);
	}
	catch (out_of_range) {
		outputErrorMessage("No channel found for ip: " + ip);
		return; 
	}
	if (channel != NULL) {
		channel->setArguments(arguments);
	}
}


// Callbacks for Covert Channels
void ChannelManager::setOutputStream(std::ostream* stream) {
	m_output_stream = stream;
}

void ChannelManager::outputMessage(string ip, string message){
	if (m_output_stream != NULL) {
		(*m_output_stream) << message;
	}
	if (m_message_callback != NULL) {
		m_message_callback(ip, message);
	}
}

void ChannelManager::setMessageCallback(function<void(string, string)> message_callback) {
	m_message_callback = message_callback;
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

UdpPacket ChannelManager::getUdpPacket(unsigned short port) {
	whisper_library::UdpPacket packet;
	packet.setSourcePort(port);
	packet.setDestinationPort(port);
	packet.setLength(11);
	packet.setChecksum(0);
	std::vector<char> data;
	data.push_back('A');
	data.push_back('B');
	data.push_back('C');
	packet.setData(data);
	return packet;
}


void ChannelManager::packetReceived(string ip, GenericPacket packet) {
	CovertChannel* channel = m_ip_mapping.at(ip);
	channel->receiveMessage(packet);
}

CovertChannel* ChannelManager::createChannel(string ip, unsigned int channel_id) {
	function<void(string)> output_message = std::bind(&ChannelManager::outputMessage, this, ip, std::placeholders::_1);
	if (channel_id == 0) {
		return new TcpHeaderCovertChannel(output_message,
			std::bind(&NetworkConnector::sendTcp, m_network, ip, std::placeholders::_1),
			std::bind(&ChannelManager::getTcpPacket, this));
	}
	// else
	return new TimingCovertChannel(output_message,
			std::bind(&NetworkConnector::sendUdp, m_network, ip, std::placeholders::_1),
			std::bind(&ChannelManager::getUdpPacket, this, std::placeholders::_1));
}

bool ChannelManager::openConnection(string ip, unsigned int channel_id) {
	if (connection(ip)) {
		outputErrorMessage("There is a connection to " + ip + " already.");
		return false;
	}
	CovertChannel* channel = createChannel(ip, channel_id);
	m_ip_mapping.insert(pair<string, CovertChannel*>(ip, channel));
	return m_network->openListener(ip, channel);
}

void ChannelManager::closeConnection(string ip) {
	m_network->closeListener(ip);
	CovertChannel* channel = m_ip_mapping.at(ip);
	delete channel;
	m_ip_mapping.erase(ip);
}

unsigned int ChannelManager::connectionCount() {
	return m_ip_mapping.size();
}

bool ChannelManager::connection(string ip) {
	try {
		m_ip_mapping.at(ip);
	}
	catch (out_of_range) {
		return false;
	}
	return true;
}

// Adapter handling
void ChannelManager::setAdapter(string name) {
	return m_network->setAdapter(name);
}

unsigned int ChannelManager::adapterCount() {
	return m_network->adapterCount();
}
vector<string> ChannelManager::networkAdapters() {
	return m_network->adapters();
}
string ChannelManager::adapterDescription(string adapter_name) {
	return m_network->adapterDescription(adapter_name);
}
vector<string> ChannelManager::adapterAddresses() {
	return m_network->adapterAddresses();
}

}
