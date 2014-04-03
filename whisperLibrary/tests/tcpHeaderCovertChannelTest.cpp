#include <tcpheadercovertchannel.hpp>
#include <genericpacket.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

struct tcpHeaderCovertChannelFixture {
	tcpHeaderCovertChannelFixture() {
		sender = NULL;
		receiver = NULL;
		sender = new whisper_library::TcpHeaderCovertChannel(std::bind(&tcpHeaderCovertChannelFixture::outputMessage, this, std::placeholders::_1),
			std::bind(&tcpHeaderCovertChannelFixture::sendToReceiver, this, std::placeholders::_1));
		receiver = new whisper_library::TcpHeaderCovertChannel(std::bind(&tcpHeaderCovertChannelFixture::outputMessage, this, std::placeholders::_1),
			std::bind(&tcpHeaderCovertChannelFixture::sendToSender, this, std::placeholders::_1));
		sender->initialize();
	}
	~tcpHeaderCovertChannelFixture() {
		delete sender;
		delete receiver;
	}
	whisper_library::TcpHeaderCovertChannel * sender;
	whisper_library::TcpHeaderCovertChannel * receiver;
	std::string test_message;
	std::stringstream output_stream;

	void outputMessage(std::string message){
		output_stream << message;
	}

	void sendToSender(whisper_library::GenericPacket packet) {
		if (sender != NULL) {
			sender->receivePacket(packet);
		}

	}
	void sendToReceiver(whisper_library::GenericPacket packet) {
		if (receiver != NULL) {
			receiver->receivePacket(packet);
		}
	}
};



BOOST_FIXTURE_TEST_SUITE(tcpHeaderCovertChannel, tcpHeaderCovertChannelFixture);

BOOST_AUTO_TEST_CASE(tcp_header_channel_send_simple_message) {
	test_message = "AB";
	sender->sendMessage(test_message);
	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

/*BOOST_AUTO_TEST_CASE(tcp_header_channel_send_long_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	test_message += "BBB";
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

BOOST_AUTO_TEST_CASE(tcp_header_channel_send_medium_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

BOOST_AUTO_TEST_CASE(tcp_header_channel_send_empty_message) {
	test_message = "";
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
} */

BOOST_AUTO_TEST_SUITE_END()