#include <tcpheadercovertchannel.hpp>
#include <genericpacket.hpp>
#include <boost/test/unit_test.hpp>
#include <string>

struct tcpHeaderCovertChannelFixture {
	tcpHeaderCovertChannelFixture() {
		uut = new whisper_library::TcpHeaderCovertChannel(std::bind(&tcpHeaderCovertChannelFixture::outputMessage, this, std::placeholders::_1),
			std::bind(&tcpHeaderCovertChannelFixture::sendPacket, this, std::placeholders::_1),
			std::bind(&tcpHeaderCovertChannelFixture::getPacket, this));
	}
	~tcpHeaderCovertChannelFixture() {
		delete uut;
	}
	whisper_library::TcpHeaderCovertChannel * uut;
	std::string test_message;
	std::stringstream output_stream;

	whisper_library::TcpPacket getPacket() {
		return whisper_library::TcpPacket();
	}


	void outputMessage(std::string message){
		output_stream << message;
	}

	void sendPacket(whisper_library::TcpPacket packet) {
		whisper_library::GenericPacket generic_packet(packet.packet());
		uut->receivePacket(generic_packet);
	}
};



BOOST_FIXTURE_TEST_SUITE(tcpHeaderCovertChannel, tcpHeaderCovertChannelFixture);

BOOST_AUTO_TEST_CASE(send_simple_message) {
	test_message = "AB";
	uut->sendMessage(test_message);
	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

BOOST_AUTO_TEST_CASE(send_long_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	test_message += "BBB";
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

BOOST_AUTO_TEST_CASE(send_medium_message) {
	test_message = "AAAAAAAAAAAAAAAAAAAAAAAA"; // 24 A's
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
}

BOOST_AUTO_TEST_CASE(send_empty_message) {
	test_message = "";
	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(test_message, output_stream.str());
} 

BOOST_AUTO_TEST_SUITE_END()