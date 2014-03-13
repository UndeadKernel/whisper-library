#include <boost/test/unit_test.hpp>
#include "../src/networkconnector.hpp"
#include <genericpacket.hpp>
#include <iostream>
#include <functional>
#include <vector>

using namespace std;

struct NetworkConnectorFixture {
	NetworkConnectorFixture() {
		network = new whisper_library::NetworkConnector(bind(&NetworkConnectorFixture::packetReceived, this, placeholders::_1, placeholders::_2));
		adapters = network->adapters();
		network->setAdapter(adapters[0].c_str());
		network->o
	}
	whisper_library::NetworkConnector * network;
	vector<string> adapters;

	void packetReceived(string ip, whisper_library::GenericPacket packet) {
		cout << "received packet from " << ip << endl;
	}
};

BOOST_FIXTURE_TEST_SUITE(networkConnector, NetworkConnectorFixture)

BOOST_AUTO_TEST_CASE(send_tcp_test) {
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
	network->sendTcp("192.168.2.104", packet);
}

BOOST_AUTO_TEST_SUITE_END()