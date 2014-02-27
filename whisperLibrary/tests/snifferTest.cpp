#include <channelmanager.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;

struct SnifferFixture {

};

BOOST_FIXTURE_TEST_SUITE(sniffer, SnifferFixture)

BOOST_AUTO_TEST_CASE(sniffer_test) {
	whisper_library::ChannelManager channelmanager;
	channelmanager.selectChannel("Timing Covert Channel");
	channelmanager.setErrorStream(&cout);
	channelmanager.setOutputStream(&cout);

	vector<char*> adapters = channelmanager.adapterNames();
	vector<char*> adapter_descriptions = channelmanager.adapterDescriptions();
	cout << "Available Adapters:" << endl;
	for (int i = 0; i < adapters.size(); i++) {
		cout << adapters[i] << ": " << adapter_descriptions[i] << endl;
	}

	channelmanager.openConnection("192.168.2.104", 23, adapters[0]);
	while (true) {

	}
}

BOOST_AUTO_TEST_SUITE_END()