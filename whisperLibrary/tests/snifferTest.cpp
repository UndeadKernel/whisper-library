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
	cout << "Available Adapters:" << endl;
	for (int i = 0; i < adapters.size(); i++) {
		cout << adapters[i] << endl;
	}
}

BOOST_AUTO_TEST_SUITE_END()