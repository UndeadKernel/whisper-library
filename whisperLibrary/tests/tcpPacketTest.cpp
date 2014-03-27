#include "../src/tcppacket.hpp"
#include <boost/test/unit_test.hpp>

#include <string>
#include <vector>
#include <bitset>
#include <iostream>

using namespace std;

struct TcpPacketFixture {
	TcpPacketFixture(){
		dut = new whisper_library::TcpPacket();
	}
	~TcpPacketFixture(){
		delete dut;
	}
	whisper_library::TcpPacket* dut;
};

BOOST_FIXTURE_TEST_SUITE(tcpPacketTest, TcpPacketFixture)


BOOST_AUTO_TEST_CASE(testIntHeaderField) {
	vector<bool> packet = dut->packet();
	dut->setDestPort(32);
	BOOST_CHECK_EQUAL(dut->destPort(), 32);
	packet = dut->packet();
	BOOST_CHECK_EQUAL(packet[26], true);
	dut->setDestPort(8080);
	BOOST_CHECK_EQUAL(dut->destPort(), 8080);
}


BOOST_AUTO_TEST_CASE(testBoolHeaderFields) {
	dut->setUrgentFlag(true);
	bitset<9> ret = dut->flags();
	BOOST_CHECK_EQUAL(ret[3], true);
} 

BOOST_AUTO_TEST_CASE(testBitsetHeaderFields) {
	std::bitset<3> inbit (std::string("100"));
	dut->setReserved(inbit);
	std::bitset<3> retbit = dut->reserved();
	std::string retstring = retbit.to_string();
	BOOST_CHECK_EQUAL(retstring, std::string("100"));
	vector<bool> packet = dut->packet();

}

BOOST_AUTO_TEST_CASE(testPacketGeneration) {
	dut->setSourcePort(1645);
	dut->setDestPort(80);
	dut->setSequenceNumber(1);
	dut->setAcknowlageNumber(1);
	dut->setDataOffset(bitset<4> ("1010"));
	dut->setSynchronisationFlag(1);
	dut->setWindowSize(128);
    dut->calculateChecksum(3225692929, 179307042, 0, 6);
    BOOST_CHECK_EQUAL(dut->checksum(), 45196);
    vector<bool> packet(dut->packet());
    whisper_library::TcpPacket test(packet);
    BOOST_CHECK_EQUAL(test.checksum(), 45196);
}

BOOST_AUTO_TEST_CASE(testImportExport){
	dut->setSourcePort(1645);
	dut->setDestPort(80);
	dut->setSequenceNumber(1);
	dut->setAcknowlageNumber(1);
	dut->setDataOffset(bitset<4>("1010"));
	dut->setSynchronisationFlag(1);
	dut->setWindowSize(128);
	dut->setReserved(bitset<3>("111"));
	dut->calculateChecksum(3225692929, 179307042, 0, 6);
	vector<bool> export_packet = dut->packet();
	whisper_library::TcpPacket test;
	test.setPacket(export_packet);
	BOOST_CHECK_EQUAL(dut->reserved(), test.reserved());
}

BOOST_AUTO_TEST_SUITE_END()
