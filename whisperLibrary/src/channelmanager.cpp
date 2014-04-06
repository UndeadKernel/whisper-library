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
#include "tcpheadercovertchannel.hpp"
#include "timingcovertchannel.hpp"
#include "packetlengthcovertchannel.hpp"
#include "udppacketgenerator.hpp"

using namespace std;

namespace whisper_library {

ChannelManager::ChannelManager() {
	m_output_stream = NULL;
	m_error_stream = NULL;
	m_network = new NetworkConnector(bind(&ChannelManager::packetReceived, this, placeholders::_1, placeholders::_2));
	// create a list of all available channels to display names and descriptions
	addChannel(new TcpHeaderCovertChannel());
	addChannel(new TimingCovertChannel());
	addChannel(new PacketLengthCovertChannel());	
}
ChannelManager::~ChannelManager() {
	// clean up open connections
	for (map<string, CovertChannel*>::iterator it = m_ip_mapping.begin(); it != m_ip_mapping.end(); it++) {
		delete it->second;
	}
	// clean up m_channels
	for (map<string, CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); it++) {
		delete it->second;
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
	for (map<string, CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		string_vector.push_back(it->second->info());
	}
	return string_vector;
}

vector<string> ChannelManager::getChannelNames() {
	vector<string> string_vector;
	for (map<string, CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		string_vector.push_back(it->second->name());
	}
	return string_vector;
}

vector<string> ChannelManager::getChannelIDs(){
	vector<string> string_vector;
	for (map<string, CovertChannel*>::iterator it = m_channels.begin(); it != m_channels.end(); ++it) {
		string_vector.push_back(it->first);
	}
	return string_vector;
}

unsigned int ChannelManager::channelCount() {
	return m_channels.size();
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

void ChannelManager::addChannel(CovertChannel* channel) {
	m_channels.insert(pair<string, CovertChannel*>(channel->id(), channel));
}



void ChannelManager::setOutputStream(std::ostream* stream) {
	m_output_stream = stream;
}

void ChannelManager::outputMessage(string ip, string message){
	if (m_output_stream != NULL) {
		(*m_output_stream) << message;
	}
	if (m_message_callback) {
		m_message_callback(ip, message);
	}
}

void ChannelManager::setMessageCallback(function<void(string, string)> message_callback) {
	m_message_callback = message_callback;
}

void ChannelManager::packetReceived(string ip, GenericPacket packet) {
	CovertChannel* channel;
	try {
		channel = m_ip_mapping.at(ip);
	}
	catch (out_of_range) {
		outputErrorMessage("Error: No connection to " + ip);
		return;
	}	
		channel->receivePacket(packet);
}

void ChannelManager::sendPacket(string ip, GenericPacket packet, string protocol){
	if (protocol.compare("tcp") == 0){
		TcpPacket tcp_packet;
		tcp_packet.setPacket(packet.packet());
		m_network->sendTcp(ip, tcp_packet);
	}
	else if (protocol.compare("udp") == 0) {
		UdpPacket udp_packet;
		udp_packet.setPacket(packet.packet());
		m_network->sendUdp(ip, udp_packet);
	}
}

CovertChannel* ChannelManager::createChannel(string ip, CovertChannel* channel) {
	function<void(string)> output_message = bind(&ChannelManager::outputMessage, this, ip, std::placeholders::_1);
	function<void(GenericPacket)> send_message = bind(&ChannelManager::sendPacket, this, ip, placeholders::_1, channel->protocol());
	CovertChannel* new_channel = channel->instance();
	new_channel->setOutput(output_message);
	new_channel->setSend(send_message);
	return new_channel;
}

// Connection
bool ChannelManager::openConnection(string ip, string channel_id) {
	if (connection(ip)) {
		outputErrorMessage("There is a connection to " + ip + " already.");
		return false;
	}
	CovertChannel* channel;
	try {
		channel = m_channels.at(channel_id);
	}
	catch (out_of_range) {
		outputErrorMessage("openConnection failed: Channel id '" + channel_id + "' unknown.");
		return false;
	}
	if (!m_network->openListener(ip, channel)) {
		return false;
	}
	CovertChannel* new_channel = createChannel(ip, channel);
	m_ip_mapping.insert(pair<string, CovertChannel*>(ip, new_channel));
	new_channel->initialize();
	return true;
}

void ChannelManager::closeConnection(string ip) {
	CovertChannel* channel;
	try {
		channel = m_ip_mapping.at(ip);
	}
	catch (out_of_range) {
		outputErrorMessage("Closing connection failed: No connection to " + ip + ".");
		return;
	}
	m_network->closeListener(ip);
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
bool ChannelManager::setAdapter(string name) {
	return m_network->setAdapter(name);
}

string ChannelManager::findValidAdapter() {
	return m_network->findValidAdapter();
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
	return m_network->adapterAddresses(m_network->currentAdapter());
}

}
