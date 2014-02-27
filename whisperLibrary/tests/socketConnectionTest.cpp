#include "../src/socketSender.hpp"
#include "../src/tcppacket.hpp"
#include "../src/udppacket.hpp"
#include <boost/test/unit_test.hpp>
#include <thread>

struct SocketTestFixture {

	SocketTestFixture(){
		sender = new whisper_library::SocketSender();
		ulong ip = 127 << 24;
		ip += 1;
	//	ulong ip = 3232236136;
		sender->setReceiverIp(ip);
	}

	~SocketTestFixture(){
		delete sender;
	}

	whisper_library::SocketSender* sender;
};

BOOST_FIXTURE_TEST_SUITE (SocketTest, SocketTestFixture)

BOOST_AUTO_TEST_CASE(sendUdpPacket) {
	/*whisper_library::UdpPacket packet;
	packet.setSourcePort(23);
	packet.setDestinationPort(23);
	packet.setLength(11);
	packet.setChecksum(33434);
	std::vector<char> data;
	data.push_back('A');
	data.push_back('B');
	data.push_back('C');
	packet.setData(data);
	sender->sendUdp(packet);*/
}

BOOST_AUTO_TEST_CASE(sendTcpPacket){
/*	uint sourcePort = 8080;
	uint destPort = 8080;
	ulong sequenceNumber = 0;
	bitset<4> dataOffset(5);
	ulong ackNumber = 0;
	uint windowSize = 2;
	vector<bool> options;
	whisper_library::TcpPacket packet(sourcePort,
					destPort,
					sequenceNumber,
					ackNumber,
					dataOffset,
					windowSize,
					options);
	packet.setAcknowledgementFlag(0);
	sender->send(packet); */
}

BOOST_AUTO_TEST_SUITE_END()
