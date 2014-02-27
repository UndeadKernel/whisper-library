#include <channelmanager.hpp>

#include <boost/test/unit_test.hpp>

using namespace std;

struct SnifferFixture {

};

BOOST_FIXTURE_TEST_SUITE(sniffer, SnifferFixture)

BOOST_AUTO_TEST_CASE(sniffer_test) {
	whisper_library::ChannelManager channelmanager;
	channelmanager.selectChannel("TCP Header Covert Channel");
	cout << "current channel: " << channelmanager.currentChannel() << endl;
	vector<char*> adapters = channelmanager.adapterNames();
	vector<char*> adapter_descriptions = channelmanager.adapterDescriptions();
	cout << "Available Adapters:" << endl;
	for (int i = 0; i < adapters.size(); i++) {
		cout << adapters[i] << ": " << adapter_descriptions[i] << endl;
	}
	channelmanager.openConnection("127.0.0.81", 8080, adapters[0]);
}

BOOST_AUTO_TEST_SUITE_END()