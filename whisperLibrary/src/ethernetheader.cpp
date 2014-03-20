#include "ethernetheader.hpp"
#include <sstream>
#include <boost/algorithm/string.hpp>
#include <iostream>

namespace whisper_library {
	EthernetHeader::EthernetHeader() {
		m_head = new unsigned char[14];
		std::fill(m_head, m_head + 14, 0);
	}
	EthernetHeader::EthernetHeader(unsigned char* header) {
		m_head = header;
	}
	EthernetHeader::~EthernetHeader() {
		delete m_head;
	}
	string EthernetHeader::destinationMAC() {
		return toMacString(m_head);
	}
	string EthernetHeader::sourceMAC() {
		return toMacString(m_head + 6);
	}
	unsigned long EthernetHeader::ethernetType() {
		return (m_head[12] << 8) + m_head[13];
	}
	void EthernetHeader::setDestinationMAC(string mac) {
		vector<string> parts;
		boost::split(parts, mac, boost::is_any_of(":"), boost::token_compress_on);
		for (unsigned int i = 0; i < 6; i++) {
			m_head[i] = hexToInt("0x"+parts[i]);
		}
	}

	void EthernetHeader::setDestinationMAC(unsigned char* mac_bitstring) {
		if (!mac_bitstring) { return; }
		memcpy(m_head, mac_bitstring, 6);
	}

	void EthernetHeader::setSourceMAC(string mac) {
		vector<string> parts;
		boost::split(parts, mac, boost::is_any_of(":"), boost::token_compress_on);
		for (unsigned int i = 0; i < 6; i++) {
			m_head[i+6] = hexToInt("0x" + parts[i]);
		}
	}

	void EthernetHeader::setSourceMAC(unsigned char* mac_bitstring) {
		if (!mac_bitstring) { return;  }
		memcpy(m_head+6, mac_bitstring, 6);
	}

	void EthernetHeader::setEthernetType(unsigned long type) {
		m_head[12] = (type >> 8);
		m_head[13] = type & 0xff;
	}

	vector<bool> EthernetHeader::toVector() {
		vector<bool> binary;
		for (unsigned int i = 0; i < 14; i++) {
			for (unsigned int j = 0; j < 8; j++) {
				unsigned char bit = m_head[i] >> (7 - j);
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

	string EthernetHeader::toString() {
		string destination_mac = "Destination MAC: " + destinationMAC() + "\n";
		string source_mac = "Source MAC: " + sourceMAC() + "\n";
		string ethernet_type = "Ethernet Type: " + to_string(ethernetType()) + "\n";
		return destination_mac + source_mac + ethernet_type;
	}

	string EthernetHeader::intToHex(unsigned int i) {
		ostringstream stream;
		stream << hex << i;
		return stream.str();
	}

	unsigned char EthernetHeader::hexToInt(string hex) {
		return stoul(hex, nullptr, 16);
	}

	string EthernetHeader::toMacString(unsigned char* buffer) {
		string mac = intToHex(buffer[0]);
		for (unsigned int i = 1; i < 6; i++) {
			mac += ":" + intToHex(buffer[i]);
		}
		return mac;
	}
}