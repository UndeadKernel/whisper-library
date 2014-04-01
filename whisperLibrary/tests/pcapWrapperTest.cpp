#include <channelmanager.hpp>
#include <thread>
#include "pcapwrapper.hpp"
#include "channelmanager.hpp"
#include "../src/networkconnector.hpp"
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <boost/asio.hpp>

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
	static const int MAXIMUM_TRY_COUNT_SENDING = 25;
	whisper_library::PcapWrapper*		wrapper;
	whisper_library::NetworkConnector*	network;
	whisper_library::ChannelManager*	manager;

	typedef struct {
		int		adapter_id;
		string	adapter_name;
		string	ipv4;
		// ...
	} ADAPTER_INFO;

	typedef struct {
		ADAPTER_INFO adapter_info;
		string destination_ipv4;
		string port;
		// ...
	} SEND_RECEIVE_DATA;

	void packetReceived(string ip, whisper_library::GenericPacket packet) { return; }

	void checkReturnCodes(vector<int> return_codes, vector<int> expected_codes, vector<int> required_codes) {
		bool expected;
		unsigned int i;
		for (int return_code : return_codes) {
			if (expected_codes.size() > 0) {
				expected = false;
				for (int expected_code : expected_codes) {
					if (return_code == expected_code) {
						expected = true;
						break;
					}
				}
				if (!expected) {
					printf("Unexpected return code: %d\n", return_code);
					BOOST_CHECK(false);
				}
			}
			for (i = 0; i < required_codes.size(); ++i) {
				if (return_code == required_codes[i]) {
					required_codes.erase(required_codes.begin() + i);
				}
			}
		}
		if (required_codes.size() > 0) {
			printf("The following required return codes weren't found:\n");
			for (int required : required_codes) {
				printf("%d\n", required);
			}
			BOOST_REQUIRE(false);
		}
	}

	void checkReturnCodes() {
		checkReturnCodes(wrapper->lastReturnCodes(), vector<int>{0}, vector<int>());
		wrapper->clearReturnCodes();
	}

	void checkReturnCodes(vector<int> expected_codes, vector<int> required_codes) {
		checkReturnCodes(wrapper->lastReturnCodes(), expected_codes, required_codes);
		wrapper->clearReturnCodes();
	}

	void checkReturnCodes(vector<int> expected_codes) {
		checkReturnCodes(wrapper->lastReturnCodes(), expected_codes, vector<int>());
		wrapper->clearReturnCodes();
	}

	bool validIPv4(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
		return (!ec) && address.is_v4() && ip.compare("0.0.0.0") != 0;
	}

	string getSourceIp(int adapter_id) {
		vector<string> addresses = wrapper->adapterAddresses(adapter_id);
		for (unsigned int i = 0; i < addresses.size(); i++) {
			if (validIPv4(addresses[i])) { return addresses[i]; }
		}
		return "0.0.0.0";
	}

	ADAPTER_INFO getAdapterInfo(int adapter_id) {
		return ADAPTER_INFO{ adapter_id, wrapper->adapterName(adapter_id), getSourceIp(adapter_id) };
	}

	ADAPTER_INFO getActiveAdapter() {
		BOOST_REQUIRE_GT(wrapper->adapterCount(), 0);
		ADAPTER_INFO active_adapter;
		bool found = false;
		for (int i = 0; i < wrapper->adapterCount(); ++i) {
			active_adapter = getAdapterInfo(i);
			if (validIPv4(active_adapter.ipv4)) {
				found = true;
				break;
			}
		}
		BOOST_REQUIRE(found); // require at least one adapter with a valid ipv4 address
		return active_adapter;
	}

	void openTestAdapter(int adapter_id, string filter) {
		wrapper->openAdapter(adapter_id, wrapper->DEFAULT_MAXPACKETSIZE, true, 1);
		if (!filter.empty()) {
			wrapper->applyFilter(adapter_id, filter);
		}
		network->setAdapter(wrapper->adapterName(adapter_id));
		checkReturnCodes();
	}
	
	SEND_RECEIVE_DATA prepareSendReceiveTest(string filter) {
		// init
		wrapper->retrieveAdapters();
		ADAPTER_INFO active_adapter = getActiveAdapter();
		int active_id				= active_adapter.adapter_id;
		string port					= "8081";
		string destination			= "192.168.178.1"; // use a random address != the source address here
		BOOST_REQUIRE(destination.compare(active_adapter.ipv4) != 0);

		openTestAdapter(active_id, filter);
		checkReturnCodes();
		return SEND_RECEIVE_DATA{active_adapter, destination, port};
	}
};

BOOST_FIXTURE_TEST_SUITE(wrapper, pcapWrapperFixture)

BOOST_AUTO_TEST_CASE(basicConstruction_test) {
	// test
	whisper_library::PcapWrapper test_wrapper = whisper_library::PcapWrapper();
	vector<int> return_codes				  = test_wrapper.lastReturnCodes();
	checkReturnCodes(return_codes, vector<int>{wrapper->NORMAL_EXECUTION}, vector<int>());
	// clean up
	test_wrapper.~PcapWrapper();
}

BOOST_AUTO_TEST_CASE(retrieveAndFreeAdapters_test) {
	// init
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

	// test
	test_wrapper.freeAdapters();
	checkReturnCodes(test_wrapper.lastReturnCodes(), vector<int>{wrapper->NORMAL_EXECUTION}, vector<int>());
	BOOST_CHECK_EQUAL(test_wrapper.adapterCount(), 0);
	test_wrapper.clearReturnCodes();
	
	test_wrapper.retrieveAdapters();
	checkReturnCodes(test_wrapper.lastReturnCodes(), vector<int>{wrapper->NORMAL_EXECUTION}, vector<int>());
	BOOST_CHECK_EQUAL(test_wrapper.adapterCount(), adapter_count);
	test_wrapper.clearReturnCodes();

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
	
	for (unsigned int i = 0; i < adapter_addresses.size(); ++i) {
		BOOST_CHECK_EQUAL_COLLECTIONS(adapter_addresses[i].begin(), adapter_addresses[i].end(), new_adapter_addresses[i].begin(), new_adapter_addresses[i].end());
	}
	BOOST_CHECK_EQUAL_COLLECTIONS(adapter_descriptions.begin(), adapter_descriptions.end(), new_adapter_descriptions.begin(), new_adapter_descriptions.end());

	// clean up
	test_wrapper.~PcapWrapper();
}

BOOST_AUTO_TEST_CASE(opencloseAdapter_test) {
	//init
	wrapper->retrieveAdapters();
	ADAPTER_INFO adapter = getActiveAdapter();
	wrapper->clearReturnCodes();

	// openAdapter Normal Execution
	wrapper->openAdapter(adapter.adapter_id, 1, false, 10);
	checkReturnCodes();

	// openAdapter Adapter already opened
	wrapper->openAdapter(adapter.adapter_id, 1, false, 10);
	checkReturnCodes(vector<int>(), vector<int>{wrapper->OPEN_ON_OPENED_HANDLE});

	// closeAdapter Normal Execution
	wrapper->closeAdapter(adapter.adapter_id);
	checkReturnCodes();

	// closeAdapter Adapter already closed
	wrapper->closeAdapter(adapter.adapter_id);
	checkReturnCodes(vector<int>(), vector<int>{wrapper->CLOSE_ON_UNOPENED_HANDLE});

	// openAdapter invalid adapter id
	wrapper->openAdapter(wrapper->adapterCount() + 1, wrapper->DEFAULT_MAXPACKETSIZE, false, 10);
	checkReturnCodes(vector<int>(), vector<int>{wrapper->ADAPTER_NOT_FOUND});

	// closeAdapter invalid adapter id
	wrapper->closeAdapter(wrapper->adapterCount() + 1);
	checkReturnCodes(vector<int>(), vector<int>{wrapper->ADAPTER_NOT_FOUND});
}

BOOST_AUTO_TEST_CASE(applyFilter_test) {
	// init
	SEND_RECEIVE_DATA data	= prepareSendReceiveTest("");
	string filter			= "src " + data.adapter_info.ipv4 + " and udp and port " + data.port + " and dst " + data.destination_ipv4;
	int active_id			= data.adapter_info.adapter_id;

	// test
	wrapper->applyFilter	(active_id, "");
	wrapper->removeFilter	(active_id);
	checkReturnCodes		();

	wrapper->applyFilter	(active_id, filter);
	network->setAdapter		(data.adapter_info.adapter_name);
	checkReturnCodes		();

	int i		  = 0;
	bool received = false;
	whisper_library::PcapWrapper::PcapPacket packet;
	while (i++ < MAXIMUM_TRY_COUNT_SENDING) {
		network->sendUdp(data.destination_ipv4, manager->getUdpPacket(8081));
		packet = wrapper->retrievePacket(active_id);
		if (packet.payload != NULL && packet.header.len != 0) {
			received = true;
			break;
		}
	}
	BOOST_CHECK(received);
	checkReturnCodes(vector<int>{wrapper->NORMAL_EXECUTION, wrapper->EMPTY_PACKET_DATA});

	// clean up
	wrapper->freeAdapters();
}

BOOST_AUTO_TEST_CASE(receivePacket_test) {
	// init
	SEND_RECEIVE_DATA data = prepareSendReceiveTest("");

	// test
	int i		  = 0;
	bool received = false;
	whisper_library::PcapWrapper::PcapPacket packet;
	while (i++ < MAXIMUM_TRY_COUNT_SENDING) {
		network->sendUdp(data.destination_ipv4, manager->getUdpPacket(8081));
		packet = wrapper->retrievePacket(data.adapter_info.adapter_id);
		if (packet.payload != NULL && packet.header.len != 0) {
			received = true;
			break;
		}
	}
	BOOST_CHECK(received);
	checkReturnCodes(vector<int>{wrapper->NORMAL_EXECUTION, wrapper->EMPTY_PACKET_DATA});
	// clean up
	wrapper->freeAdapters();
}

#ifdef WIN32
BOOST_AUTO_TEST_CASE(win32SendPacket_test) {
	// init
	SEND_RECEIVE_DATA data = prepareSendReceiveTest("");

	unsigned char packet_buffer[100];
	int buffer_length = 100;

	// ethernet
	/* mac destination to 6:5:4:3:2:1 */
	packet_buffer[0] = 6;
	packet_buffer[1] = 5;
	packet_buffer[2] = 4;
	packet_buffer[3] = 3;
	packet_buffer[4] = 2;
	packet_buffer[5] = 1;

	/* mac sourc 1:2:3:4:5:6 */
	packet_buffer[6] = 1;
	packet_buffer[7] = 2;
	packet_buffer[8] = 3;
	packet_buffer[9] = 4;
	packet_buffer[10] = 5;
	packet_buffer[11] = 6;

	/* Fill the rest */
	for (int i = 12; i < 100; i++) { packet_buffer[i] = i % 256; }

	string filter = "ether src 01:02:03:04:05:06 and ether dst 06:05:04:03:02:01";
	wrapper->applyFilter(data.adapter_info.adapter_id, filter);

	// test
	int i = 0;
	bool received = false;
	whisper_library::PcapWrapper::PcapPacket packet;
	while (i++ < MAXIMUM_TRY_COUNT_SENDING) {
		// Wireshark display filter: eth.addr == 6:5:4:3:2:1
		wrapper->sendPacket(data.adapter_info.adapter_id, packet_buffer, buffer_length);
		packet = wrapper->retrievePacket(data.adapter_info.adapter_id);
		if (packet.payload != NULL && packet.header.len != 0) {
			received = true;
			break;
		}
	}
	BOOST_CHECK(received);
	checkReturnCodes(vector<int>{wrapper->NORMAL_EXECUTION, wrapper->EMPTY_PACKET_DATA});
	// clean up
	wrapper->freeAdapters();
	
}
#endif

BOOST_AUTO_TEST_SUITE_END()
