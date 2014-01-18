#include <whisperLibrary/tcppacket.hpp>
#include <boost/test/unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>

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

// testing an integer input
BOOST_AUTO_TEST_CASE(testIntHeaderField) {
	dut->setDestPort(50);
	BOOST_CHECK_EQUAL(dut->destPort(), 50);

}

// testing a flag input
BOOST_AUTO_TEST_CASE(testBoolHeaderFields) {
    dut->setUrgentFlag(true);
    BOOST_CHECK_EQUAL(dut->urgentFlag(), true);
} 

// testing a bitset input
BOOST_AUTO_TEST_CASE(testBitsetHeaderFields) {
    bitset<3> inbit (string("101"));
	dut->setReserved(inbit);
    bitset<3> retbit = dut->reserved();
    string retstring = retbit.to_string();
    BOOST_CHECK_EQUAL(retstring, string("101"));
}

// Testing the packet generation with a given checksum to make sure it works as inteded
BOOST_AUTO_TEST_CASE(testPacketGeneration) {
	dut->setSourcePort(1645);
	dut->setDestPort(80);
	dut->setSequenceNumber(1);
	dut->setAcknowlageNumber(1);
    bitset<4> off (string("1010"));
	dut->setDataOffset(off);
    bitset<3> res (string("000"));
	dut->setReserved(res);
    bitset<9> flag (string("010000000"));
	dut->setFlags(flag);
	dut->setWindowSize(128);
	dut->setChecksum(0);
	dut->setUrgentPointer(0);
	ulong sourceIP;
	sourceIP = (192 << 24) + (68 << 16) + (43 << 8) + 1;
	ulong destIP;
	destIP = (10 << 24) + (176 << 16) + (2 << 8) + 34;
    dut->calculateChecksum(sourceIP, destIP, 0, 6);
    uint expchecksum = 45196;
	uint checks = dut->checksum();
	BOOST_CHECK_EQUAL(expchecksum, checks);
	vector<bool> tcp_packet (dut->packet());
}

BOOST_AUTO_TEST_SUITE_END()
