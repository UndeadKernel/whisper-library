#include <channelmanager.hpp>
#include <thread>
#include "pcapwrapper.hpp"
#include "../src/networkconnector.hpp"
#include <boost/test/unit_test.hpp>

using namespace std;

struct pcapWrapperFixture {
	pcapWrapperFixture() {
		wrapper = whisper_library::PcapWrapper();
		network = new whisper_library::NetworkConnector(bind(&pcapWrapperFixture::packetReceived, this, placeholders::_1, placeholders::_2));
	}
	~pcapWrapperFixture() {
		wrapper.~PcapWrapper();
		delete network;
	}
	whisper_library::PcapWrapper wrapper;
	whisper_library::NetworkConnector * network;

	void packetReceived(string ip, whisper_library::GenericPacket packet) {}
};

BOOST_FIXTURE_TEST_SUITE(wrapper, pcapWrapperFixture)

BOOST_AUTO_TEST_CASE(pcapWrapper_test) {

}

BOOST_AUTO_TEST_SUITE_END()