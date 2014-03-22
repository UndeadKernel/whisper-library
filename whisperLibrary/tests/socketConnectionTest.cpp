#include "../src/socketSender.hpp"
#include "../include/whisperLibrary/pcapwrapper.hpp"
#include "../src/tcppacket.hpp"
#include "../src/udppacket.hpp"
#include "../src/ipheaderv4.hpp"
#include "../include/whisperLibrary/genericpacket.hpp"
#include <boost/test/unit_test.hpp>
#include <thread>

struct SocketTestFixture {

	SocketTestFixture(){
		sender = new whisper_library::SocketSender();
		pcap = new whisper_library::PcapWrapper();

	}

	~SocketTestFixture(){
		delete sender;
		delete pcap;
	}

	whisper_library::SocketSender* sender;
	whisper_library::PcapWrapper* pcap;

	void openAllAdapters() {
		unsigned int count = pcap->adapterCount();
		for (unsigned int i = 0; i < count; i++) {
			if (pcap->openAdapter(i, pcap->DEFAULT_MAXPACKETSIZE, true, 10) == 0) { //successfully opened
				adapter_ids.push_back(i);
			}
		}
	}

	void setFilter(string ip, string protocol, unsigned short port) {
		string filter = "host " + ip + " and port " + to_string(port) + " and " + protocol;
		for (unsigned int i = 0; i < adapter_ids.size(); i++) {
			pcap->applyFilter(adapter_ids[i], filter.c_str());
		}
	}

	whisper_library::GenericPacket retrievePacket() {
		vector<bool> frame;
		for (unsigned int j = 0; j < 100; j++) { // try 100 times
			for (unsigned int i = 0; i < adapter_ids.size(); i++) {
				frame = pcap->retrievePacketAsVector(adapter_ids[i]);
				if (!frame.empty()) {
					whisper_library::GenericPacket udp = extractApplicationLayer(frame);
					return udp;
				}
			}
		}
		whisper_library::GenericPacket empty_packet;
		return empty_packet;
	}

	whisper_library::GenericPacket extractApplicationLayer(vector<bool> frame) {
		whisper_library::GenericPacket generic_packet;
		vector<bool> packet_little_endian;
		unsigned int length_bit;
		vector<bool> application_layer;
		packet_little_endian = switchEndian(frame);

		whisper_library::IpHeaderv4 ip_header(packet_little_endian);
		length_bit = ip_header.ipHeaderLength() * 32;

		application_layer.insert(application_layer.begin(), packet_little_endian.begin() + length_bit + 112, packet_little_endian.end());
		generic_packet.setContent(application_layer);

		return generic_packet;
	}

	vector<bool> switchEndian(vector<bool> binary) {
		vector<bool> switched_endian;
		for (unsigned int i = 0; i < binary.size() - 7; i = i + 8) {
			vector<bool> byte;
			for (unsigned int j = 0; j < 8; j++) {
				byte.push_back(binary[i + (7 - j)]);
			}
			switched_endian.insert(switched_endian.end(), byte.begin(), byte.end());
		}
		return switched_endian;
	}
	
	vector<unsigned int> adapter_ids;
};

BOOST_FIXTURE_TEST_SUITE (SocketTest, SocketTestFixture)

BOOST_AUTO_TEST_CASE(sendUdpPacket) {
	string destination_ip = "20.20.20.20";
	unsigned short port = 23;

	openAllAdapters();
	setFilter(destination_ip, "udp", port);

	whisper_library::UdpPacket packet;
	packet.setSourcePort(port);
	packet.setDestinationPort(port);
	packet.setLength(11);
	packet.setChecksum(0);
	std::vector<char> data;
	data.push_back('A');
	data.push_back('B');
	data.push_back('C');
	packet.setData(data);
	sender->sendUdp(destination_ip, packet);
	whisper_library::GenericPacket received_packet = retrievePacket();
	whisper_library::UdpPacket received_udp_packet;
	received_udp_packet.setPacket(received_packet.content());

	BOOST_CHECK(!received_udp_packet.packet().empty());
	BOOST_CHECK_EQUAL(received_udp_packet.sourcePort(), packet.sourcePort());
	BOOST_CHECK_EQUAL(received_udp_packet.destinationPort(), packet.destinationPort());
	BOOST_CHECK_EQUAL(received_udp_packet.length(), packet.length());
	BOOST_CHECK_EQUAL(received_udp_packet.sourcePort(), packet.sourcePort());
	BOOST_CHECK_EQUAL(received_udp_packet.data().size(), packet.data().size());
	vector<char> received_data = received_udp_packet.data();
	if (received_data.size() == data.size()) {
		for (unsigned int i = 0; i < data.size(); i++) {
			BOOST_CHECK_EQUAL(received_data[i], data[i]);
		}
	}
}

BOOST_AUTO_TEST_CASE(sendTcpPacket){
/*	uint sourcePort = 8080;
	uint destPort = 8080;
	ulong sequenceNumber = 1;
	bitset<4> dataOffset("1010");
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
	packet.setSynchronisationFlag(1);
	sender->sendTcp(packet); */
}

BOOST_AUTO_TEST_SUITE_END()
