#include <boost/test/unit_test.hpp>
#include "../src/networkconnector.hpp"
#include <genericpacket.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include "../src/ethernetheader.hpp"
#include "../include/whisperLibrary/tcpheadercovertchannel.hpp"
#include <boost/asio.hpp>

using namespace std;

struct NetworkConnectorFixture {
	NetworkConnectorFixture() {
		network = new whisper_library::NetworkConnector(bind(&NetworkConnectorFixture::packetReceived, this, placeholders::_1, placeholders::_2));
		adapters = network->adapters();
		channel = new whisper_library::TcpHeaderCovertChannel(bind(&NetworkConnectorFixture::output, this, placeholders::_1),
															  bind(&NetworkConnectorFixture::send, this, placeholders::_1),
															  bind(&NetworkConnectorFixture::getPacket, this));
		received = false;
	}
	~NetworkConnectorFixture() {
		delete network;
		delete channel;
	}
	whisper_library::NetworkConnector * network;
	whisper_library::TcpHeaderCovertChannel * channel;

	vector<string> adapters;
	whisper_library::GenericPacket received_packet;
	bool received;

	void packetReceived(string ip, whisper_library::GenericPacket packet) {
		received_packet = packet;
		received = true;
	}
	bool validIPv4(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
		return (!ec) && address.is_v4() && ip.compare("0.0.0.0") != 0;
	}

	string getSourceAddress() {
		vector<string> addresses = network->adapterAddresses();
		for (unsigned int i = 0; i < addresses.size(); i++) {
			if (validIPv4(addresses[i])) {
				return addresses[i];
			}
		}
	}
	unsigned int findSendAdapter() {
		for (unsigned int i = 0; i < network->adapterCount(); i++) {
			network->setAdapter(adapters[i]);
			vector<string> addresses = network->adapterAddresses();
			for (unsigned j = 0; j < addresses.size(); j++) {
				if (validIPv4(addresses[j])) {
					return i;
				}
			}
		}
	}

	void output(string message) {};
	void send(whisper_library::TcpPacket packet) {};
	whisper_library::TcpPacket getPacket() { return whisper_library::TcpPacket(); };
};

BOOST_FIXTURE_TEST_SUITE(networkConnector, NetworkConnectorFixture)

#ifdef WIN32
BOOST_AUTO_TEST_CASE(send_tcp_win32_test) {
	unsigned int adapter_id = findSendAdapter();
	network->setAdapter(adapters[adapter_id]);
	string source_ip = getSourceAddress();
	network->openListener(source_ip, channel); //loopback

	unsigned short port = 8080;
	unsigned long sequenceNumber = 1;
	bitset<4> dataOffset("1010");
	unsigned long ackNumber = 0;
	unsigned int windowSize = 2;
	vector<bool> options;
	whisper_library::TcpPacket packet(port,
		port,
		sequenceNumber,
		ackNumber,
		dataOffset,
		windowSize,
		options);
	packet.setAcknowledgementFlag(0);
	packet.setSynchronisationFlag(1);

	network->sendTcp(source_ip, packet); //send packet to own ip

	//wait for packet
	unsigned int i = 0;
	while (!received && i < 5) {
		i++;
		this_thread::sleep_for(chrono::seconds(1));
	}

	whisper_library::TcpPacket received_tcp;
	received_tcp.setPacket(received_packet.content());
	BOOST_CHECK_EQUAL(packet.packet().size(), received_tcp.packet().size());
	BOOST_CHECK_EQUAL(packet.sourcePort(), received_tcp.sourcePort());
	BOOST_CHECK_EQUAL(packet.destPort(), received_tcp.destPort());
	BOOST_CHECK_EQUAL(packet.sourcePort(), received_tcp.sourcePort());
	BOOST_CHECK_EQUAL(packet.checksum(), received_tcp.checksum());
	vector<bool> packet_data = packet.data();
	vector<bool> received_data = received_tcp.data();
	if (packet_data.size() == received_data.size()) {
		for (unsigned int i = 0; i < packet_data.size(); i++) {
			BOOST_CHECK_EQUAL(packet_data[i], received_data[i]);
		}
	}

	network->closeListener(source_ip);
}
#endif

BOOST_AUTO_TEST_SUITE_END()