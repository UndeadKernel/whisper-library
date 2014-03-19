#include <boost/test/unit_test.hpp>
#include "../src/networkconnector.hpp"
#include <genericpacket.hpp>
#include <iostream>
#include <functional>
#include <vector>
#include "../src/ethernetheader.hpp"

using namespace std;

struct NetworkConnectorFixture {
	NetworkConnectorFixture() {
		network = new whisper_library::NetworkConnector(bind(&NetworkConnectorFixture::packetReceived, this, placeholders::_1, placeholders::_2));
		adapters = network->adapters();

	}
	~NetworkConnectorFixture() {
		delete network;
	}
	whisper_library::NetworkConnector * network;
	vector<string> adapters;

	void packetReceived(string ip, whisper_library::GenericPacket packet) {
		cout << "received packet from " << ip << endl;
	}
};

BOOST_FIXTURE_TEST_SUITE(networkConnector, NetworkConnectorFixture)

BOOST_AUTO_TEST_CASE(get_destination_mac) {
	whisper_library::EthernetHeader header;
	header.setSourceMAC(network->getDestinationMAC("192.168.2.105", "192.168.2.1"));
	cout << header.toString() << endl;
}

BOOST_AUTO_TEST_SUITE_END()