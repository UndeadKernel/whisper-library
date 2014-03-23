#include <channelmanager.hpp>
#include <thread>
#include "pcapwrapper.hpp"
#include "channelmanager.hpp"
#include "../src/networkconnector.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>

using namespace std;

struct pcapWrapperFixture {
	pcapWrapperFixture() {
		wrapper = new whisper_library::PcapWrapper();
		network = new whisper_library::NetworkConnector(bind(&pcapWrapperFixture::packetReceived, this, placeholders::_1, placeholders::_2));
		manager = new whisper_library::ChannelManager();
	}
	~pcapWrapperFixture() {
		delete wrapper;
		delete network;
		delete manager;
	}
	whisper_library::PcapWrapper*		wrapper;
	whisper_library::NetworkConnector*	network;
	whisper_library::ChannelManager*	manager;

	void packetReceived(string ip, whisper_library::GenericPacket packet) { return; }
	void checkReturnCodes(vector<int> return_codes) {
		for (int return_code : return_codes) {
			BOOST_CHECK_EQUAL(return_code, 0);
		}
	}
};

BOOST_FIXTURE_TEST_SUITE(wrapper, pcapWrapperFixture)

BOOST_AUTO_TEST_CASE(basicConstruction_test) {
	whisper_library::PcapWrapper test_wrapper = whisper_library::PcapWrapper();
	vector<int> return_codes = test_wrapper.lastReturnCodes();
	checkReturnCodes(return_codes);
	test_wrapper.~PcapWrapper();
}

BOOST_AUTO_TEST_CASE(retrieveAndFreeAdapters_test) {
	// since retrieveAdapters is called with the constructor so we make sure to take a fresh wrapper-object here
	whisper_library::PcapWrapper test_wrapper = whisper_library::PcapWrapper();
	BOOST_REQUIRE(test_wrapper.adapterCount() > 0);

	int adapter_id;
	int adapter_count				= test_wrapper.adapterCount();
	vector<string> adapter_names	= test_wrapper.adapterNames();
	vector<string> new_adapter_names;

	vector< vector<string> >		adapter_addresses;
	vector< vector<string> >		new_adapter_addresses;

	vector<string>					adapter_descriptions; 
	vector<string>					new_adapter_descriptions;
	
	for ( string name : adapter_names ) {
		adapter_id = test_wrapper.adapterId(name, test_wrapper.ADAPTER_NAME);
		adapter_addresses.push_back(test_wrapper.adapterAddresses(adapter_id));
		adapter_descriptions.push_back(test_wrapper.adapterDescription(adapter_id));
		/*cout << "ID: " << adapter_id << ", Description: " << test_wrapper.adapterDescription(adapter_id) << "Addresses:\n";
		for ( string address : test_wrapper.adapterAddresses(adapter_id) ) {
			cout << address << '\n';
		}*/
	}

	test_wrapper.freeAdapters();
	checkReturnCodes(test_wrapper.lastReturnCodes());
	BOOST_CHECK_EQUAL(test_wrapper.adapterCount(), 0);
	
	test_wrapper.retrieveAdapters();
	checkReturnCodes(test_wrapper.lastReturnCodes());
	BOOST_CHECK_EQUAL(test_wrapper.adapterCount(), adapter_count);

	new_adapter_names = test_wrapper.adapterNames();
	BOOST_CHECK_EQUAL_COLLECTIONS(adapter_names.begin(), adapter_names.end(), new_adapter_names.begin(), new_adapter_names.end());

	for (string name : new_adapter_names) {
		adapter_id = test_wrapper.adapterId(name, test_wrapper.ADAPTER_NAME);
		new_adapter_addresses.push_back(test_wrapper.adapterAddresses(adapter_id));
		new_adapter_descriptions.push_back(test_wrapper.adapterDescription(adapter_id));
		/*cout << "ID: " << adapter_id << ", Description: " << test_wrapper.adapterDescription(adapter_id) << "Addresses:\n";
		for (string address : test_wrapper.adapterAddresses(adapter_id)) {
			cout << address << '\n';
		}*/
}
	
	for (int i = 0; i < adapter_addresses.size(); ++i) {
		BOOST_CHECK_EQUAL_COLLECTIONS(adapter_addresses[i].begin(), adapter_addresses[i].end(), new_adapter_addresses[i].begin(), new_adapter_addresses[i].end());
	}
	BOOST_CHECK_EQUAL_COLLECTIONS(adapter_descriptions.begin(), adapter_descriptions.end(), new_adapter_descriptions.begin(), new_adapter_descriptions.end());
}

BOOST_AUTO_TEST_CASE(applyFilter_test) {
	BOOST_REQUIRE_GT(wrapper->adapterCount(), 0);
	string name = wrapper->adapterName(0);
	string dest = "192.168.178.1";
	wrapper->openAdapter(name, wrapper->DEFAULT_MAXPACKETSIZE, false, 1);
	wrapper->applyFilter(name, "");
	wrapper->removeFilter(name);
	checkReturnCodes(wrapper->lastReturnCodes());

	int i;
	vector<string> addresses = wrapper->adapterAddresses(0);
	string filter = "";
	for (i = 0; i < addresses.size(); i++) {
		filter.append("src " + addresses[i] + " ");
		if (i != (addresses.size() - 1)) {
			filter.append("or ");
		}
	}
	filter.append("and udp and port 8081 and dst " + dest);
	wrapper->applyFilter(name, filter);
	network->setAdapter(name);

	i = 0;
	bool received = false;
	whisper_library::PcapWrapper::PcapPacket packet;
	while (i++ < 25) {
		network->sendUdp(dest, manager->getUdpPacket(8081));
		packet = wrapper->retrievePacket(name);
		if (packet.payload != NULL && packet.header.len != 0) {
			received = true;
			break;
		}
	}
	BOOST_CHECK(received);
	checkReturnCodes(wrapper->lastReturnCodes());
}

BOOST_AUTO_TEST_SUITE_END()