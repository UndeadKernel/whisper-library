#include <boost/test/unit_test.hpp>
#include "packetlengthcovertchannel.hpp"
#include "../../src/udppacket.hpp"
#include "../../src/udppacketgenerator.hpp"

struct PacketLengthCovertChannelFixture {
	PacketLengthCovertChannelFixture(){
		m_channel = new whisper_library::PacketLengthCovertChannel(
			std::bind(&PacketLengthCovertChannelFixture::output,this,std::placeholders::_1),
			std::bind(&PacketLengthCovertChannelFixture::send, this, std::placeholders::_1),
			std::bind(&PacketLengthCovertChannelFixture::getUdpPacket, this, std::placeholders::_1));
	}

	~PacketLengthCovertChannelFixture(){
		delete m_channel;
	}

	void output(std::string message){
		m_received = message;
	}

	void send(whisper_library::UdpPacket packet){
		whisper_library::GenericPacket gpacket;
		gpacket.setPacket(packet.packet());
		m_channel->receivePacket(gpacket);
	}

	whisper_library::UdpPacket getUdpPacket(int length){
		 return whisper_library::UdpPacketGenerator::generatePacketWithLength(23,length);	
	}

	std::string m_received;
	whisper_library::PacketLengthCovertChannel* m_channel;
};

BOOST_FIXTURE_TEST_SUITE(packetLengthCovertChannelTest, PacketLengthCovertChannelFixture)

BOOST_AUTO_TEST_CASE(testEmptyMessage){
	std::string msg = "";
	m_channel->sendMessage(msg);
	BOOST_CHECK_EQUAL(msg, m_received);
}

BOOST_AUTO_TEST_CASE(testShortMessage){
	std::string msg = "test";
	m_channel->sendMessage(msg);
	BOOST_CHECK_EQUAL(msg, m_received);
}

BOOST_AUTO_TEST_CASE(testLongMessage){
	std::string msg = "a longer test to see if the packet length covert channel works correct";
	m_channel->sendMessage(msg);
	BOOST_CHECK_EQUAL(msg, m_received);
}

BOOST_AUTO_TEST_SUITE_END()