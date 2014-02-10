#include <boost/test/unit_test.hpp>
#include <timingcovertchannel.hpp>
#include <channelmanager.hpp>
#include <chrono>
#include <boost/algorithm/string.hpp>

struct timingCovertChannelFixture {
};

BOOST_FIXTURE_TEST_SUITE(timingCovertChannel, timingCovertChannelFixture);

void outputMessage(string message) {
	cout << message << endl;
}

void sendPacket(chrono::high_resolution_clock::time_point start, whisper_library::TcpPacket & packet) {
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<uint, milli> time_elapsed = chrono::duration_cast<chrono::duration<uint, milli>>(end - start);
	cout << time_elapsed.count() << "ms" << endl;
}

whisper_library::TcpPacket getTcpPacket() {
	return whisper_library::TcpPacket();
}


BOOST_AUTO_TEST_CASE(timing_send_simple_message) {
/*	chrono::steady_clock::time_point start = chrono::steady_clock::now();
	whisper_library::TimingCovertChannel timing_cc(&outputMessage, std::bind(&sendPacket, start, std::placeholders::_1), &getTcpPacket);
	timing_cc.sendMessage("a");

	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	chrono::duration<uint, milli> time_elapsed = chrono::duration_cast<chrono::duration<uint, milli>>(end - start);

	this_thread::sleep_for(chrono::seconds(5));
	cout << "test ended after: " << time_elapsed.count()<< "ms" << endl;*/
}

BOOST_AUTO_TEST_CASE(timing_receive_simple_message) {
	string test_message = "Ein Test.";

	whisper_library::ChannelManager channelmanager;
	stringstream received_message;
	channelmanager.selectChannel(1);
	channelmanager.setOutputStream(&received_message);

	channelmanager.sendMessage(test_message);

	//Wait for end of timeout
	this_thread::sleep_for(chrono::seconds(10));
	BOOST_CHECK_EQUAL(boost::to_upper_copy(test_message), received_message.str());
}


BOOST_AUTO_TEST_SUITE_END()