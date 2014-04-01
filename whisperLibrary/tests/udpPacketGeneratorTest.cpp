#include <boost/test/unit_test.hpp>
#include "../src/udppacketgenerator.hpp"

struct UdpPacketGeneratorFixture {

};

BOOST_FIXTURE_TEST_SUITE(udpPacketGenerator, UdpPacketGeneratorFixture);

BOOST_AUTO_TEST_CASE(udp_generate_random_length_test) {
	whisper_library::UdpPacket packet = whisper_library::UdpPacketGenerator::generateNextPacket(30000);
	BOOST_CHECK_EQUAL(packet.destinationPort(), 30000);
	BOOST_CHECK_EQUAL(packet.sourcePort(), 30000);
	BOOST_CHECK_EQUAL(packet.checksum(), 0);
	vector<char> data = packet.data();
	BOOST_CHECK_EQUAL(packet.length(), data.size() + 8);
	BOOST_CHECK(data.size() >= 42 && data.size() <= 142);
}

BOOST_AUTO_TEST_CASE(udp_generate_fixed_length_test) {
	whisper_library::UdpPacket packet = whisper_library::UdpPacketGenerator::generatePacketWithLength(30000, 100);
	BOOST_CHECK(packet.data().size() == 100-8);
}

BOOST_AUTO_TEST_SUITE_END()