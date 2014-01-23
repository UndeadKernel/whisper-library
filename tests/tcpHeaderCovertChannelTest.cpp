#include <whisperLibrary\channelmanager.hpp>
#include <boost\test\unit_test.hpp>
#include <string>

struct tcpHeaderCovertChannelFixture {
	whisper_library::ChannelManager channelmanager;
	std::string test_message;
};

BOOST_FIXTURE_TEST_SUITE(tcpHeaderCovertChannel, tcpHeaderCovertChannelFixture);

BOOST_AUTO_TEST_CASE(send_simple_message) {
	test_message = "AB";
	channelmanager.sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, channelmanager.getOutputMessage());
}

BOOST_AUTO_TEST_CASE(send_long_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	test_message += "BBB";
	channelmanager.sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, channelmanager.getOutputMessage());
}

BOOST_AUTO_TEST_CASE(send_medium_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	channelmanager.sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, channelmanager.getOutputMessage());
}

BOOST_AUTO_TEST_CASE(send_empty_message) {
	test_message = "";
	channelmanager.sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, channelmanager.getOutputMessage());
}

BOOST_AUTO_TEST_SUITE_END()