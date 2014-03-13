#include "ipheaderv4.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

namespace whisper_library {
	IpHeaderv4::IpHeaderv4() {
		m_head = new unsigned char[20];
		std::fill(m_head, m_head + 20, 0);
	}

	IpHeaderv4::IpHeaderv4(vector<bool> frame) {
		m_head = new unsigned char[20];

		int buffer_size = frame.size() / 8;
		unsigned char* packet_buffer = static_cast<unsigned char*>(malloc(buffer_size));

		for (unsigned int i = 0; i < buffer_size; i++) {
			packet_buffer[i] = 0; // initialize
			for (unsigned int j = 0; j < 8; j++) {
				packet_buffer[i] |= (frame[j + (i * 8)] ? 1 : 0) << (7 - j);
			}
		}

		memcpy(m_head, packet_buffer + 14, 20);
		free(packet_buffer);
	}

	void IpHeaderv4::calculateHeaderLength() {
		m_head[0] = 5 + (m_head[0] & 0xf0);
	}
	void IpHeaderv4::calculateChecksum() {
		m_head[10] = 0;
		m_head[11] = 0;
		unsigned long sum = 0;
		for (unsigned int i = 0; i < 19; i = i+2) {
			sum += (m_head[i] << 8) + m_head[i+1];
			unsigned int carry = sum >> 16;
			if (carry == 1) {
				sum = (sum & 0xffff) + 1;
			}
		}
		m_head[10] = (sum >> 8) & 0xff;
		m_head[11] = sum & 0xff;
	}

	unsigned int IpHeaderv4::version() {
		return m_head[0] >> 4;
	}
	unsigned int IpHeaderv4::ipHeaderLength() {
		return m_head[0] & 0x0f;
	}
	unsigned int IpHeaderv4::typeOfService() {
		return m_head[1];
	}
	unsigned int IpHeaderv4::totalLength() {
		return (m_head[2] << 8) + m_head[3];
	}
	unsigned int IpHeaderv4::identification() {
		return (m_head[4] << 8) + m_head[5];
	}
	bool IpHeaderv4::flagReserved() {
		unsigned int flag = m_head[6] >> 7;
		if (flag == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	bool IpHeaderv4::flagDontFragment() {
		unsigned int flag = m_head[6] >> 6;
		flag = flag & 0x1;
		if (flag == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	bool IpHeaderv4::flagFragmented() {
		unsigned int flag = m_head[6] >> 5;
		flag = flag & 0x1;
		if (flag == 1) {
			return true;
		}
		else {
			return false;
		}
	}
	unsigned int IpHeaderv4::offset() {
		return ((m_head[6] << 8) + m_head[7]) & 0x1fff; // cut off 3 bit flag
	}
	unsigned int IpHeaderv4::timeToLive() {
		return m_head[8];
	}
	unsigned int IpHeaderv4::protocol() {
		return m_head[9];
	}
	unsigned int IpHeaderv4::checksum() {
		return (m_head[10] << 8) + m_head[11];
	}
	unsigned long IpHeaderv4::sourceIp() {
		return (m_head[12] << 24) + (m_head[13] << 16) + (m_head[14] << 8) + m_head[15];
	}
	string IpHeaderv4::sourceIpDotted() {
		unsigned long ip = sourceIp();
		return ipToDotted(ip);
	}
	unsigned long IpHeaderv4::destinationIp() {
		return (m_head[16] << 24) + (m_head[17] << 16) + (m_head[18] << 8) + m_head[19];
	}
	string IpHeaderv4::destinationIpDotted() {
		unsigned long ip = destinationIp();
		return ipToDotted(ip);
	}

	string IpHeaderv4::ipToDotted(unsigned long ip) {
		vector<unsigned int> parts;
		parts.push_back((ip & 0xff000000) >> 24);
		parts.push_back((ip & 0x00ff0000) >> 16);
		parts.push_back((ip & 0x0000ff00) >> 8);
		parts.push_back((ip & 0x000000ff));
		return (to_string(parts[0]) + "." + to_string(parts[1]) + "." + to_string(parts[2]) + "." + to_string(parts[3]));
	}

	void IpHeaderv4::setVersion(unsigned int version) {
		m_head[0] = (version << 4) + (m_head[0] & 0x0f);
	}
	void IpHeaderv4::setTypeOfService(unsigned int tos) {
		m_head[1] = tos;
	}
	void IpHeaderv4::setTotalLength(unsigned int total_length) {
		m_head[2] = (total_length >> 8) & 0xff;
		m_head[3] = total_length & 0xff;
	}
	void IpHeaderv4::setIdentification(unsigned int identification) {
		m_head[4] = (identification >> 8) & 0xff;
		m_head[5] = identification & 0xff;
	}
	void IpHeaderv4::setFlagReserved(bool flag) {
		if (flag) {
			m_head[6] = (1 << 7) | m_head[6];
		}
		else {
			m_head[6] = m_head[6] & 0x7f;
		}
	}
	void IpHeaderv4::setFlagdontFragment(bool flag) {
		if (flag) {
			m_head[6] = (1 << 6) | m_head[6];
		}
		else {
			m_head[6] = m_head[6] & 0xbf;
		}
	}
	void IpHeaderv4::setFlagFragmented(bool flag) {
		if (flag) {
			m_head[6] = (1 << 5) | m_head[6];
		}
		else {
			m_head[6] = m_head[6] & 0xdf;
		}
	}
	void IpHeaderv4::setOffset(unsigned int offset) {
		m_head[6] = ((offset >> 8) & 0xff) + (m_head[6] & 0xe0);
		m_head[7] = offset;
	}
	void IpHeaderv4::setTimeToLive(unsigned int ttl) {
		m_head[8] = ttl;
	}
	void IpHeaderv4::setProtocol(unsigned int protocol_id) {
		m_head[9] = protocol_id;
	}
	void IpHeaderv4::setSourceIp(unsigned long source_ip) {
		m_head[12] = (source_ip >> 24) & 0xff;
		m_head[13] = (source_ip >> 16) & 0xff;
		m_head[14] = (source_ip >> 8) & 0xff;
		m_head[15] = source_ip & 0xff;
	}
	void IpHeaderv4::setSourceIp(string source_ip) {
		setSourceIp(ipToUlong(source_ip));
	}
	void IpHeaderv4::setDestinationIp(unsigned long destination_ip) {
		m_head[16] = (destination_ip >> 24) & 0xff;
		m_head[17] = (destination_ip >> 16) & 0xff;
		m_head[18] = (destination_ip >> 8) & 0xff;
		m_head[19] = destination_ip & 0xff;
	}
	void IpHeaderv4::setDestinationIp(string destination_ip) {
		setDestinationIp(ipToUlong(destination_ip));
	}

	unsigned long IpHeaderv4::ipToUlong(string ip) {
		vector<string> parts;
		boost::split(parts, ip, boost::is_any_of("."), boost::token_compress_on);
		unsigned long ulong_ip = 0;
		try {
			ulong_ip += boost::lexical_cast<unsigned int>(parts[3]);
			ulong_ip += boost::lexical_cast<unsigned int>(parts[2]) << 8;
			ulong_ip += boost::lexical_cast<unsigned int>(parts[1]) << 16;
			ulong_ip += boost::lexical_cast<unsigned int>(parts[0]) << 24;
		}
		catch (boost::bad_lexical_cast e) {
			
		}

		return ulong_ip;
	}

	void IpHeaderv4::print() {
		for (unsigned int i = 0; i < 20; i++) {
			if (i % 8 == 0) {
			printf(" ");
			}
			if (i % 2 == 0) {
			printf(" ");
			}
			if (i % 16 == 0) {
			printf("\n");
			}
			printf("%.2X", m_head[i]);
		}
	}

	string IpHeaderv4::info() {
		string version = "IP Version: " + to_string(this->version()) + "\n";
		string ip_length = "IP Header Lenght: " + to_string((ipHeaderLength() * 32)) + " bit" + "\n";
		string tos = "Type of service: " + to_string(typeOfService()) + "\n";
		string total_length = "ip packet length: " + to_string(totalLength()) + " byte" + "\n";
		string identification = "Identification " + to_string(this->identification()) + "\n";
		string flags = "Flags: " + to_string(flagReserved()) + " " + to_string(flagDontFragment()) + " " + to_string(flagFragmented()) + "\n";
		string offset = "Offset: " + to_string(this->offset()) + "\n";
		string ttl = "Time to live: " + to_string(timeToLive()) + "\n";
		string protocol = "Protocol: " + to_string(this->protocol()) + "\n";
		string checksum = "Checksum: " + to_string(this->checksum()) + "\n";
		string source = "Source IP: " + sourceIpDotted() + "\n";
		string destination = "Destination IP: " + destinationIpDotted() + "\n";

		return version + ip_length + tos + total_length + identification + flags + offset + ttl + protocol + checksum + source + destination;
	}

	vector<bool> IpHeaderv4::toVector() {
		vector<bool> binary;
		for (unsigned int i = 0; i < 20; i++) {
			unsigned char byte = m_head[i];
			for (unsigned int j = 0; j < 8; j++) {
				unsigned char bit = byte >> (7 - j);
				bit = bit & 0x01;
				if (bit == 0) {
					binary.push_back(false);
				}
				else {
					binary.push_back(true);
				}
			}
		}
		return binary;
	}
}