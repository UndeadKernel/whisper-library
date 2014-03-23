/*	<pcapwrapper.cpp>
Copyright(C) 2013,2014  Jan Simon Bunten
						Simon Kadel
						Martin Sven Oehler
						Arne Sven Stühlmeyer

This File is part of the WhisperLibrary

WhisperLibrary is free software : you can redistribute it and / or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

WhisperLibrary is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include <pcapwrapper.hpp>

namespace whisper_library {
	PcapWrapper::PcapWrapper() {
		// initialize
		DEBUG(2, "PcapWrapper construction.\n");
		m_last_return_codes = boost::circular_buffer<int>(RETURN_CODE_BUFFER_SIZE);
		m_adapter_raw_data = NULL;
		retrieveAdapters();
	}

	PcapWrapper::~PcapWrapper() {
		DEBUG(2, "PcapWrapper destruction.\n");
		freeAdapters();
		m_last_return_codes.clear();
	}
	
	inline bool PcapWrapper::checkForAdapterId(int adapter_id) {
		if (m_adapter_data.empty()) {
			// unsafe call
			fprintf(stderr, "Warning: Adapter requested while all adapters are freed. Retrieving adapters...");
			retrieveAdapters();
		}
		if (static_cast<int>(m_adapter_data.size()) <= adapter_id) { // ID starts with 0
			// specified adapter not found
			fprintf(stderr, "Error: No network device with ID #%d\n", adapter_id);
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), false);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), true);
	}

	std::string PcapWrapper::adapterName(int adapter_id) {
		if (!checkForAdapterId(adapter_id)) {
			// specified adapter not found
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), NULL);
		} 
		RETURN_VALUE(RC(NORMAL_EXECUTION), m_adapter_data[adapter_id][ADAPTER_NAME]);
	}

	std::vector<std::string> PcapWrapper::adapterNames() {
		/* we could use getAdapterName(adapter_id) here, 
		but constant checking for the adapter_id would be a unnecessary waste of performance */
		std::vector<std::string> ret;
		for (std::vector<std::string> adapter : m_adapter_data) {
			ret.push_back(adapter[ADAPTER_NAME]);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), ret);
	}

	int PcapWrapper::adapterId(std::string value, int key, bool increment_key) {
		int i, j;
		for (i = 0; i < static_cast<int>(m_adapter_data.size()); ++i) {
			for (j = key; j < static_cast<int>(m_adapter_data[i].size()) && increment_key || j == key; ++j) {
				if (value.compare(m_adapter_data[i][j]) == 0) {
					RETURN_VALUE(RC(NORMAL_EXECUTION), i);
				}
			}
		}
		RETURN_CODE(RC(ADAPTER_NOT_FOUND));
	}

	int PcapWrapper::adapterId(std::string adapter_value, int value_type) {
		return adapterId(adapter_value, value_type, (value_type == ADAPTER_ADDRESS ? true : false));
	}

	std::vector<std::string> PcapWrapper::adapterAddresses(int adapter_id) {
		std::vector<std::string> ret;
		if (!checkForAdapterId(adapter_id)) { return ret; }  // specified adapter not found
		for (unsigned int i = ADAPTER_ADDRESS; i < (m_adapter_data[adapter_id]).size(); ++i) {
			ret.push_back(m_adapter_data[adapter_id][i]);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), ret);
	}

	std::string	PcapWrapper::adapterDescription(int adapter_id) {
		if (!checkForAdapterId(adapter_id)) {
			// specified adapter not found
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), NULL);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), m_adapter_data[adapter_id][ADAPTER_DESCRIPTION]);
	}

	int PcapWrapper::adapterCount() {
		return static_cast<int>(m_adapter_data.size());
	}

	int PcapWrapper::retrieveAdapters() {
		freeAdapters(); // free potentially opened adapters
		char error_buffer[PCAP_ERRBUF_SIZE]; // pcap error buffer

		// find all openable devices
		if (pcap_findalldevs(&m_adapter_raw_data, error_buffer) == -1) { 
			fprintf(stderr, "Error: Failed to retrieve network adapters: %s\n", error_buffer);
			RETURN_CODE(RC(ERROR_RETRIEVING_ADAPTERS));
		}
		if ( m_adapter_raw_data == NULL ) {
			fprintf(stderr, "Error: No network adapters found or insufficient permissions.\n");
			RETURN_CODE(RC(NO_ADAPTERS_FOUND));
		}
		for (pcap_if_t* adapter = m_adapter_raw_data; adapter; adapter = adapter->next) {
			/* Only include adapters with a pcap address
				
				This prevents inactive or slave devices (aggregation/bonding slaves) from being added
				and we only get the intermediate drivers instead.
			*/
			if (!adapter->addresses) { continue; }

			std::vector<std::string> current_adapter;
			current_adapter.push_back(adapter->name);					// adapter/device name in the system (for example /dev/eth0)
			current_adapter.push_back(adapter->description);			// adapter descriptional name (usually the product name of the network card o.s.)
			bpf_u_int32 netmask;
			bpf_u_int32 ip4_address;
			if (pcap_lookupnet(adapter->name, &netmask, &ip4_address, error_buffer) < 0) {
				netmask = 0;
			}
			m_adapter_netmasks.push_back(netmask);

			DEBUG(1, "Device Name: %s\nDescription: %s\nFlags: #%d\n", adapter->name, adapter->description, adapter->flags);
			// get adapter addresses
			char* address_buffer;
			size_t buffer_size = 0;
			for (pcap_addr* address = adapter->addresses; address; address = address->next) {
				buffer_size = 0;
				switch (address->addr->sa_family) {
					// address families with 32 bit addresses 
					case AF_INET: {		// UDP, TCP, ...
						buffer_size = 16; // usual size of a ipv4 string representation
						break;
					}
					// address families with 128 bit addresses 
					case AF_INET6: {	// IPv6
						buffer_size = 45; // usual size of a ipv6 string representation
						break;
					}
					// unknown or unspecified families
					default: {
						DEBUG(1, "Device Address Unknown\n\n");
						// Currently no point in adding adapter addresses of unknown families.
						continue;
					}
				}
				// deallocation in freeAdapters
				buffer_size		*= sizeof(char); // byte size
				address_buffer	= static_cast<char*>(malloc(buffer_size));
				if (!address_buffer) {
					fprintf(stderr, "Error: Out of memory.\n");
					RETURN_CODE(RC(OUT_OF_MEMORY));
				}
				// currently only addresses up to 128bit and known to getnameinfo are supported by ipToString
				ipToString(address->addr, address_buffer, buffer_size);
				if (address_buffer) {
					current_adapter.push_back(std::string(address_buffer));
				}
				free(address_buffer);
			}
			// Set global adapter_data
			m_adapter_data.push_back(current_adapter);
		}
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int PcapWrapper::openAdapter(int adapter_id, int max_packet_size, bool promiscuous_mode, int timeout) {
		if (!checkForAdapterId(adapter_id)) { RETURN_CODE(RC(ADAPTER_NOT_FOUND)); } // specified adapter not found
		// open handle
		char error_buffer[PCAP_ERRBUF_SIZE]; // pcap error buffer

		// resize handles vector as needed
		if (static_cast<int>(m_adapter_handles.size()) <= adapter_id) {
			m_adapter_handles.resize(adapter_id + 1);
		}
		// open handle for live capturing
		m_adapter_handles[adapter_id] = pcap_open_live(m_adapter_data[adapter_id][0].c_str(), max_packet_size, (promiscuous_mode ? 1 : 0 ), timeout, error_buffer);
		if (!m_adapter_handles[adapter_id]) {
			fprintf(stderr, "Failed to open handle on network device #%d (%s)\n%s\n", adapter_id, m_adapter_data[adapter_id][0].c_str(), error_buffer);
			RETURN_CODE(RC(ERROR_OPENING_HANDLE));
		}
		DEBUG(1, "Capturing started on device #%d\n", adapter_id);
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int PcapWrapper::openAdapter(std::string adapter_name, int max_packet_size, bool promiscuous_mode, int timeout) {
		return openAdapter(adapterId(adapter_name, ADAPTER_NAME), max_packet_size, promiscuous_mode, timeout);
	}

	int PcapWrapper::closeAdapter(std::string adapter_name) {
		return closeAdapter(adapterId(adapter_name, ADAPTER_NAME));
	}

	int PcapWrapper::closeAdapter(int adapter_id) {
		if (static_cast<int>(m_adapter_handles.size()) > adapter_id && m_adapter_handles[adapter_id]) {
			pcap_close(m_adapter_handles[adapter_id]);
			m_adapter_handles[adapter_id] = NULL;
			RETURN_CODE(RC(NORMAL_EXECUTION));
		}
		// Nothing to close
		RETURN_CODE(RC(CLOSE_ON_UNOPENED_HANDLE));
	}

	int PcapWrapper::freeAdapters() {
		if (m_adapter_raw_data == NULL) {
			// nothing to free
			RETURN_CODE(RC(NORMAL_EXECUTION));
		}
		for (pcap_t* handle : m_adapter_handles) {
			if (handle) { pcap_close(handle); }
		}
		// clear vector content
		m_adapter_data.clear();
		m_adapter_handles.clear();
		m_adapter_netmasks.clear();
		// free adapters
		pcap_freealldevs( m_adapter_raw_data );
		// purge old values
		m_adapter_raw_data	= NULL;
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int PcapWrapper::applyFilter(int adapter_id, std::string filter) {
		
		if (!checkForAdapterId(adapter_id)) { return -1; } // specified adapter not found
		struct bpf_program	filter_compiled;
		pcap_t*				handle = NULL;
		if (static_cast<int>(m_adapter_handles.size()) > adapter_id) {
			handle = m_adapter_handles[adapter_id];
		}
		if (!handle) {
			fprintf(stderr, "Error: applyFilter() called on unopened adapter.\n");
			RETURN_CODE(RC(ACCESS_ON_UNOPENED_HANDLE));
		}
		bpf_u_int32 netmask;
		if (m_adapter_netmasks.size() > adapter_id && m_adapter_netmasks[adapter_id] != 0) {
			netmask = m_adapter_netmasks[adapter_id];
		} else {
			netmask = PCAP_NETMASK_UNKNOWN;
		}

		if (pcap_compile(handle, &filter_compiled, filter.c_str(), 1, netmask) < 0) {
			fprintf(stderr, "Error: Failed to compile given filter.\n");
			RETURN_CODE(RC(ERROR_COMPILING_FILTER));
		}
		if (pcap_setfilter(handle, &filter_compiled) < 0) {
			fprintf(stderr, "Error: Failed to apply the given filter.\n");
			RETURN_CODE(RC(ERROR_APPLYING_FILTER));
		}
		DEBUG(1, "Filter successfully applied.\nFilter: %s\n", filter.c_str());
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int PcapWrapper::applyFilter(std::string adapter_name, std::string filter) {
		return applyFilter(adapterId(adapter_name, ADAPTER_NAME), filter);
	}

	int PcapWrapper::removeFilter(std::string adapter_name) {
		return applyFilter(adapter_name, "");
	}

	int PcapWrapper::removeFilter(int adapter_id) {
		return applyFilter(adapter_id, "");
	}

	PcapWrapper::PcapPacket PcapWrapper::retrievePacket(int adapter_id) {
		PcapPacket packet = { NULL, NULL };
		if (!checkForAdapterId(adapter_id)) {
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), packet); // specified adapter not found
		}
		/*
			struct pcap_pkthdr:
				struct timeval	ts; - system dependent size (either 32 or 64bit)
				bpf_u_int32		caplen;
				bpf_u_int32		len;
		*/
		struct pcap_pkthdr*	packet_header;
		const u_char*		packet_data; 
		pcap_t*				handle		= NULL;
		int					iterations	= 0;
		int					return_code = 0;
		if (static_cast<int>(m_adapter_handles.size()) > adapter_id) {
			handle = m_adapter_handles[adapter_id];
		}
		if (!handle) {
			fprintf(stderr, "Error: retrievePacket() called on unopened adapter.\n");
			RETURN_VALUE(RC(ACCESS_ON_UNOPENED_HANDLE), packet);
		}
		/* Note: 
			packet_data can contain NULL if
			1. an error occured
			2. no packets were read from live capture (due to read timeout) in 500 tries
			3. no packet passed the filter
			Quote from winpcap doc: "Unfortunately, there is no way to determine whether an error occured or not."
			( http://www.winpcap.org/docs/docs_412/html/group__wpcapfunc.html#gadf60257f650aaf869671e0a163611fc3 )
		*/
		while ( (return_code = pcap_next_ex(handle, &packet_header, &packet_data)) == 0 && ++iterations <= 500);
		DEBUG(3, "Packet data: %u - length: %d\n", (packet_data ? reinterpret_cast<const unsigned int*>(packet_data) : 0), packet_header->len);
		if (return_code == 1) {
			packet.payload = packet_data;
			packet.header = *packet_header;
		}
		RETURN_VALUE(RC((return_code == 1 ? NORMAL_EXECUTION : EMPTY_PACKET_DATA)), packet);
	}

	PcapWrapper::PcapPacket PcapWrapper::retrievePacket(std::string adapter_name) {
		return retrievePacket(adapterId(adapter_name, ADAPTER_NAME));
	}

	std::vector<bool> PcapWrapper::retrievePacketAsVector(int adapter_id) {
		std::vector<bool> bitVector;
		PcapPacket packet			= retrievePacket(adapter_id);
		if (packet.payload == NULL) {
			RETURN_VALUE(RC(EMPTY_PACKET_DATA), bitVector);
		}
		const unsigned char* it		= packet.payload;
		unsigned int i, j;
		for (i = 0; i < packet.header.len; ++i, ++it) {
			for (j = 0; j < 8; ++j) {
				bitVector.push_back((*it & (1 << j)) != 0);
			}
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), bitVector);
	}

	std::vector<bool> PcapWrapper::retrievePacketAsVector(std::string adapter_name) {
		return retrievePacketAsVector(adapterId(adapter_name, ADAPTER_NAME));
	}

	inline char* PcapWrapper::ipToString(struct sockaddr* socket_address, char* buffer, size_t buffer_length) {
		if (!socket_address) { return NULL; }
		// sockaddr_storage requires Windows XP, Windows Server 2003 or later
		socklen_t address_length = sizeof(struct sockaddr_storage);
		DEBUG(2, "ipToString()\nBuffer length: %d; Address Length: %d\n", static_cast<unsigned int>(buffer_length), static_cast<unsigned int>(address_length));
		
		getnameinfo(socket_address, address_length, buffer, buffer_length, NULL, 0, NI_NUMERICHOST);
		DEBUG(2, "Address from getnameinfo: %s\n\n", (buffer) ? buffer : "NULL");
		return (buffer);
	}

	std::vector<int> PcapWrapper::lastReturnCodes() {
		std::vector<int> returnCodes = std::vector<int>(20);
		for (int rc : m_last_return_codes) {
			returnCodes.push_back(rc);
		}
		return returnCodes;
	}

	int	PcapWrapper::sendPacket(int adapter_id, unsigned char* packet_buffer, int buffer_size) {
	#ifdef WIN32
		if (!checkForAdapterId(adapter_id)) {
			// specified adapter not found
			RETURN_CODE(RC(ADAPTER_NOT_FOUND));
		}
		pcap_t*	handle = NULL;
		if (static_cast<int>(m_adapter_handles.size()) > adapter_id) {
			handle = m_adapter_handles[adapter_id];
		}
		if (!handle) {
			fprintf(stderr, "Error: retrievePacket() called on unopened adapter.\n");
			RETURN_CODE(RC(ACCESS_ON_UNOPENED_HANDLE));
		}
		if (pcap_sendpacket(handle, packet_buffer, buffer_size ) < 0) {
			fprintf(stderr, "Error: Failed to send the given packet: \n", pcap_geterr(handle));
			RETURN_CODE(RC(UNSPECIFIED_ERROR_OCCURED));
		}
		RETURN_CODE(RC(NORMAL_EXECUTION));
	#else
		fprintf(stderr, "Error: Wrong function called. pcap_sendpacket(...) only works with WinPcap.\n");
		RETURN_CODE(RC(UNSPECIFIED_ERROR_OCCURED));
	#endif
	}

	int	PcapWrapper::sendPacket(std::string adapter_name, unsigned char* packet_buffer, int buffer_size) {
		return sendPacket(adapterId(adapter_name, ADAPTER_NAME), packet_buffer, buffer_size);
	}

	int	PcapWrapper::sendPacket(int adapter_id, std::vector<bool> packet_data) {
		if (packet_data.size() < 1) { RETURN_CODE(RC(NORMAL_EXECUTION)); } // nothing to send
		int buffer_size			= packet_data.size() / 8;
		unsigned char* buffer	= static_cast<unsigned char*>(malloc(buffer_size));
		if (!buffer) {
			fprintf(stderr, "Error: Out of memory.\n");
			RETURN_CODE(RC(OUT_OF_MEMORY));
		}

		// convert logical values to numerical
		int i, j, return_code;
		for (i = 0; i < buffer_size; ++i) {
			buffer[i] = 0; // initialize
			for (j = 0; j < 8; ++j) {
				DEBUG(4, "bit index: %d, data index: %d, value: %d\n", j, j + (i * 8), (packet_data[j + (i * 8)] ? 1 : 0));
				buffer[i] |= (packet_data[j + (i * 8)] ? 1 : 0) << j;
			}
			DEBUG(3, "send buffer char #%d content: %u\n\n", i, buffer[i]);
		}
		return_code = sendPacket(adapter_id, buffer, buffer_size);
		free(buffer); // important!
		RETURN_CODE(return_code);
	}

	int	PcapWrapper::sendPacket(std::string adapter_name, std::vector<bool> packet_data) {
		return sendPacket(adapterId(adapter_name, ADAPTER_NAME), packet_data);
	}
} 