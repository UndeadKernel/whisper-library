#include <boost/test/unit_test.hpp>
#include "../src/tcppacketgenerator.hpp"
struct TcpPacketGeneratorFixture {
	TcpPacketGeneratorFixture() {
		sender = new whisper_library::TcpPacketGenerator(8080, std::bind(&TcpPacketGeneratorFixture::sendToReceiver, this, placeholders::_1),
															   std::bind(&TcpPacketGeneratorFixture::forward, this, placeholders::_1));
		receiver = new whisper_library::TcpPacketGenerator(8080, std::bind(&TcpPacketGeneratorFixture::sendToSender, this, placeholders::_1),
			std::bind(&TcpPacketGeneratorFixture::forward, this, placeholders::_1));
	}
	~TcpPacketGeneratorFixture() {
		delete sender;
		delete receiver;
	}

	void sendToSender(whisper_library::TcpPacket packet) {
		cout << "sending to sender ";
		sender->receivePacket(packet);
	}

	void sendToReceiver(whisper_library::TcpPacket packet) {
		cout << "sending to receiver ";
		receiver->receivePacket(packet);
	}

	void forward(whisper_library::GenericPacket packet) {

	}

	whisper_library::TcpPacketGenerator* sender;
	whisper_library::TcpPacketGenerator* receiver;
};

BOOST_FIXTURE_TEST_SUITE(tcpPacketGenerator, TcpPacketGeneratorFixture);

BOOST_AUTO_TEST_CASE(tcp_handshake_test) {
	sender->sendConnect();
	BOOST_CHECK_EQUAL(sender->status(), whisper_library::TcpPacketGenerator::ESTABLISHED);
	BOOST_CHECK_EQUAL(receiver->status(), whisper_library::TcpPacketGenerator::ESTABLISHED);
}

BOOST_AUTO_TEST_SUITE_END()