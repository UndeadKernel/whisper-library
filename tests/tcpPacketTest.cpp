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

}

BOOST_AUTO_TEST_SUITE_END()
