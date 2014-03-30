#include "../src/socketSender.hpp"
#include "../include/whisperLibrary/pcapwrapper.hpp"
#include "../src/tcppacket.hpp"
#include "../src/udppacket.hpp"
#include "../src/ipheaderv4.hpp"
#include "../include/whisperLibrary/genericpacket.hpp"
#include <boost/test/unit_test.hpp>
#include <thread>
#include <boost/asio.hpp>

struct SocketSenderTestFixture {

	SocketSenderTestFixture(){
		sender = new whisper_library::SocketSender();
		pcap = new whisper_library::PcapWrapper();
	}

	~SocketSenderTestFixture(){
		delete sender;
		delete pcap;
	}

	void openAllAdapters() {
		unsigned int count = pcap->adapterCount();
		for (unsigned int i = 0; i < count; i++) {
			if (pcap->openAdapter(i, pcap->DEFAULT_MAXPACKETSIZE, true, 10) == 0) { //successfully opened
				adapter_ids.push_back(i);
			}
		}
	}

	void closeAllAdapters() {
		for (unsigned int i = 0; i < adapter_ids.size(); i++) {
			pcap->closeAdapter(adapter_ids[i]);
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
		vector<bool> application_layer;
		vector<bool> packet_little_endian = switchEndian(frame);

		whisper_library::IpHeaderv4 ip_header(packet_little_endian);
		unsigned int length_bit = ip_header.ipHeaderLength() * 32;
		unsigned int total_length_bit = ip_header.totalLength() * 8;

		application_layer.insert(application_layer.begin(), packet_little_endian.begin() + 112 + length_bit, packet_little_endian.begin() + 112 + total_length_bit); // cut of 14 byte ethernet header (112 bit) and ip header and padding
		whisper_library::GenericPacket generic_packet;
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

	string getSourceIp() {
		for (unsigned int j = 0; j < adapter_ids.size(); j++) {
			vector<string> addresses = pcap->adapterAddresses(adapter_ids[j]);
			for (unsigned int i = 0; i < addresses.size(); i++) {
				if (validIPv4(addresses[i])) {
					return addresses[i];
				}
			}
		}
		return "0.0.0.0";
	}

	bool validIPv4(string ip) {
		boost::system::error_code ec;
		boost::asio::ip::address address = boost::asio::ip::address::from_string(ip, ec);
		return (!ec) && address.is_v4() && ip.compare("0.0.0.0") != 0;
	}
	
	vector<unsigned int> adapter_ids;
	whisper_library::SocketSender* sender;
	whisper_library::PcapWrapper* pcap;
};

BOOST_FIXTURE_TEST_SUITE(socketSenderTest, SocketSenderTestFixture)

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

	closeAllAdapters();
}

#ifndef WIN32
BOOST_AUTO_TEST_CASE(sendTcpPacket){
	openAllAdapters();
	string source_ip = getSourceIp();
	string destination_ip = "20.20.20.20";
	unsigned short port = 8080;
	unsigned long sequenceNumber = 1;
	bitset<4> dataOffset("1010");
	unsigned long ackNumber = 0;
	unsigned int windowSize = 2;
	vector<bool> options;
	whisper_library::TcpPacket packet(port,
					port,
					sequenceNumber,
					ackNumber,
					dataOffset,
					windowSize,
					options);
	packet.setAcknowledgementFlag(0);
	packet.setSynchronisationFlag(1);
	packet.calculateChecksum(boost::asio::ip::address_v4::from_string(source_ip).to_ulong(),
							 boost::asio::ip::address_v4::from_string(destination_ip).to_ulong(), 0, 6);
	setFilter(destination_ip, "tcp", port);
	sender->sendTcp(source_ip, destination_ip, packet); 
	whisper_library::GenericPacket received_packet = retrievePacket();
	whisper_library::TcpPacket received_tcp;
	received_tcp.setPacket(received_packet.content());
	BOOST_CHECK_EQUAL(packet.packet().size(), received_tcp.packet().size());
	BOOST_CHECK_EQUAL(packet.sourcePort(), received_tcp.sourcePort());
	BOOST_CHECK_EQUAL(packet.destPort(), received_tcp.destPort());
	BOOST_CHECK_EQUAL(packet.sourcePort(), received_tcp.sourcePort());
	BOOST_CHECK_EQUAL(packet.checksum(), received_tcp.checksum());
	vector<bool> packet_data = packet.data();
	vector<bool> received_data = received_tcp.data();
	if (packet_data.size() == received_data.size()) {
		for (unsigned int i = 0; i < packet_data.size(); i++) {
			BOOST_CHECK_EQUAL(packet_data[i], received_data[i]);
		}
	}

	closeAllAdapters();
}
#endif

BOOST_AUTO_TEST_SUITE_END()
