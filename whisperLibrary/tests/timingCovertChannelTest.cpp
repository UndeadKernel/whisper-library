#include <boost/test/unit_test.hpp>
#include <timingcovertchannel.hpp>
#include <chrono>

struct timingCovertChannelFixture {
};

BOOST_FIXTURE_TEST_SUITE(timingCovertChannel, timingCovertChannelFixture);

void outputMessage(string message) {
	cout << message << endl;
}

void sendPacket(chrono::high_resolution_clock::time_point start, whisper_library::TcpPacket & packet) {
	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<double, milli> time_elapsed = chrono::duration_cast<chrono::duration<double, milli>>(end - start);
	cout << time_elapsed.count() << "ms" << endl;
}

whisper_library::TcpPacket getTcpPacket() {
	return whisper_library::TcpPacket();
}


BOOST_AUTO_TEST_CASE(timing_send_simple_message) {
	using namespace std::placeholders;

	chrono::high_resolution_clock::time_point start = chrono::high_resolution_clock::now();
	whisper_library::TimingCovertChannel timing_cc(&outputMessage, std::bind(&sendPacket, start, std::placeholders::_1), &getTcpPacket);
	timing_cc.sendMessage("a");

	chrono::high_resolution_clock::time_point end = chrono::high_resolution_clock::now();
	chrono::duration<double, milli> time_elapsed = chrono::duration_cast<chrono::duration<double, milli>>(end - start);

	cout << "test ended after: " << time_elapsed.count()<< "ms" << endl;
}


BOOST_AUTO_TEST_SUITE_END()