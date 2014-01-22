#include <whisperLibrary\channelmanager.hpp>
#include <boost\test\unit_test.hpp>

struct tcpHeaderCovertChannelFixture {
	whisper_library::ChannelManager channelmanager;
};

BOOST_FIXTURE_TEST_SUITE(tcpHeaderCovertChannel, tcpHeaderCovertChannelFixture);

BOOST_AUTO_TEST_CASE(send_simple_message) {
	//channelmanager.sendMessage("AB");
}

BOOST_AUTO_TEST_SUITE_END()