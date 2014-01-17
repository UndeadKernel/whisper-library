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


BOOST_AUTO_TEST_CASE(testIntHeaderFields) {
	dut->setDestPort(50);
	BOOST_CHECK_EQUAL(dut->destPort(), 50);
}


BOOST_AUTO_TEST_CASE(testBoolHeaderFields) {

} 

BOOST_AUTO_TEST_CASE(testBitsetHeaderFields) {

}

BOOST_AUTO_TEST_CASE(testPrivateFunctions) {

}

BOOST_AUTO_TEST_SUITE_END()
