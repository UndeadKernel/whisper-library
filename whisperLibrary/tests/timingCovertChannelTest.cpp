#include <timingcovertchannel.hpp>
#include <boost/test/unit_test.hpp>
#include <chrono>
#include <boost/algorithm/string.hpp>

struct timingCovertChannelFixture {
	timingCovertChannelFixture() {
		uut = new whisper_library::TimingCovertChannel(std::bind(&timingCovertChannelFixture::outputMessage, this, std::placeholders::_1),
			std::bind(&timingCovertChannelFixture::sendPacket, this, std::placeholders::_1),
			std::bind(&timingCovertChannelFixture::getPacket, this));
	}
	~timingCovertChannelFixture() {
		delete uut;
	}

	whisper_library::TimingCovertChannel * uut;
	string test_message;
	stringstream received_message;

	whisper_library::UdpPacket getPacket() {
		return whisper_library::UdpPacket();
	}

	void outputMessage(std::string message){
		received_message << message;
	}

	void sendPacket(whisper_library::UdpPacket packet) {
		whisper_library::GenericPacket generic_packet;
		uut->receivePacket(generic_packet);
	}
};

BOOST_FIXTURE_TEST_SUITE(timingCovertChannel, timingCovertChannelFixture);

BOOST_AUTO_TEST_CASE(timingcc_send_simple_message) {
	test_message = "Ein Test.";

	uut->sendMessage(test_message);

	//Wait for end of transfer
	this_thread::sleep_for(chrono::seconds(3));
	BOOST_CHECK_EQUAL(boost::to_upper_copy(test_message), received_message.str());
}

BOOST_AUTO_TEST_CASE(timingcc_send_empty_message) {
	test_message = "";

	uut->sendMessage(test_message);

	BOOST_CHECK_EQUAL(boost::to_upper_copy(test_message), received_message.str());
}

BOOST_AUTO_TEST_SUITE_END()