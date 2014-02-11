#include <boost/test/unit_test.hpp>
#include <channelmanager.hpp>
#include <chrono>
#include <boost/algorithm/string.hpp>

struct timingCovertChannelFixture {
	timingCovertChannelFixture() {
		channelmanager.selectChannel(1);
		channelmanager.setOutputStream(&received_message);
	}
	whisper_library::ChannelManager channelmanager;
	string test_message;
	stringstream received_message;
};

BOOST_FIXTURE_TEST_SUITE(timingCovertChannel, timingCovertChannelFixture);

BOOST_AUTO_TEST_CASE(timingcc_send_simple_message) {
	test_message = "Ein Test.";

	channelmanager.sendMessage(test_message);

	//Wait for end of transfer
	this_thread::sleep_for(chrono::seconds(10));
	BOOST_CHECK_EQUAL(boost::to_upper_copy(test_message), received_message.str());
}

BOOST_AUTO_TEST_CASE(timingcc_send_empty_message) {
	test_message = "";

	channelmanager.sendMessage(test_message);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(test_message), received_message.str());
}

BOOST_AUTO_TEST_SUITE_END()