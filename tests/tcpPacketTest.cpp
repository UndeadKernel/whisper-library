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


BOOST_AUTO_TEST_CASE(testIntHeaderField) {
	dut->setDestPort(50);
	BOOST_CHECK_EQUAL(dut->destPort(), 50);

}


BOOST_AUTO_TEST_CASE(testBoolHeaderFields) {
	dut->setUrg(true);
	BOOST_CHECK_EQUAL(dut->urg(), true);
} 

BOOST_AUTO_TEST_CASE(testBitsetHeaderFields) {
	std::bitset<3> inbit (std::string("101"));
	dut->setReserved(inbit);
	std::bitset<3> retbit = dut->reserved();
	std::string retstring = retbit.to_string();
	BOOST_CHECK_EQUAL(retstring, std::string("101"));
}


BOOST_AUTO_TEST_CASE(testPacketGeneration) {
	dut->setSourcePort(1645);
	dut->setDestPort(80);
	dut->setSequenceNumber(1);
	dut->setAcknowlageNumber(1);
	std::bitset<4> off (std::string("0101"));
	dut->setDataOffset(off);
	std::bitset<3> res (std::string("000"));
	dut->setReserved(res);
	std::bitset<9> flag (std::string("000000010"));
	dut->setFlags(flag);
	dut->setWindowSize(128);
	dut->setChecksum(0);
	dut->setUrgentPointer(0);
	ulong sourceIP;
	sourceIP = (192 << 24) + (68 << 16) + (43 << 8) + 1;
	ulong destIP;
	destIP = (10 << 24) + (176 << 16) + (2 << 8) + 34;
	cout << "starting checksum calc\n";
	dut->calculateChecksum(sourceIP, destIP, 0, 6);
	uint expchecksum = 20339;
	uint checks = dut->checksum();
	BOOST_CHECK_EQUAL(expchecksum, checks);
	vector<bool> tcp_packet (dut->packet());
}

BOOST_AUTO_TEST_SUITE_END()
