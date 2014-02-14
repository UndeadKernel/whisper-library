#include <sniffer.hpp>

namespace whisper_library {

	Sniffer::Sniffer() {
		// initialize
		DEBUG(2, "Sniffer construction.\n");
		m_last_return_codes = boost::circular_buffer<int>(RETURN_CODE_BUFFER_SIZE);
		m_adapter_raw_data = NULL;
		retrieveAdapters();
	}

	Sniffer::~Sniffer() {
		DEBUG(2, "Sniffer destruction.\n");
		freeAdapters();
		m_last_return_codes.clear();
	}
	
	bool Sniffer::checkForAdapterId(unsigned int adapter_id) {
		if (m_adapter_data.empty()) {
			// unsafe call
			fprintf(stderr, "Warning: Adapter requested while all adapters are freed. Retrieving adapters...");
			retrieveAdapters();
		}
		if (m_adapter_data.size() <= adapter_id) { // ID starts with 0
			// specified adapter not found
			fprintf(stderr, "Error: No network device with ID #%d\n", adapter_id);
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), false);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), true);
	}

	char* Sniffer::adapterName(unsigned int adapter_id) {
		if (!checkForAdapterId(adapter_id)) {
			// specified adapter not found
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), NULL);
		} 
		RETURN_VALUE(RC(NORMAL_EXECUTION), m_adapter_data[adapter_id][ADAPTER_NAME]);
	}

	std::vector<char*> Sniffer::adapterNames() {
		/* we could use getAdapterName(adapter_id) here, 
		but constant checking for the adapter_id would be a unnecessary waste of performance */
		std::vector<char*> ret;
		for (std::vector<char*> adapter : m_adapter_data) {
			ret.push_back(adapter[ADAPTER_NAME]);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), ret);
	}

	int Sniffer::adapterId(char* value, unsigned int key, bool increment_key) {
		unsigned int i, j;
		for (i = 0; i < m_adapter_data.size(); i++) {
			for (j = key; j < m_adapter_data[i].size() && increment_key || j == key; j++) {
				if (m_adapter_data[i][j] == value) {
					RETURN_VALUE(RC(NORMAL_EXECUTION), i);
				}
			}
		}
		RETURN_CODE(RC(ADAPTER_NOT_FOUND));
	}

	unsigned int Sniffer::adapterId(char* adapter_value, unsigned int value_type) {
		return adapterId(adapter_value, value_type, (value_type == ADAPTER_ADDRESS ? true : false));
	}

	std::vector<char*> Sniffer::adapterAddresses(unsigned int adapter_id) {
		std::vector<char*> ret;
		if (!checkForAdapterId(adapter_id)) { return ret; }  // specified adapter not found
		for (unsigned int i = ADAPTER_ADDRESS; i < (m_adapter_data[adapter_id]).size(); i++) {
			ret.push_back(m_adapter_data[adapter_id][i]);
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), ret);
	}

	unsigned int Sniffer::adapterCount() {
		return static_cast<unsigned int>(m_adapter_data.size());
	}

	int Sniffer::retrieveAdapters() {
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

			std::vector<char*> current_adapter;
			current_adapter.push_back(adapter->name);		 // adapter/device name in the system (for example /dev/eth0)
			current_adapter.push_back(adapter->description); // adapter descriptional name (usually the product name of the network card o.s.)
			DEBUG(1, "Device Name: %s\nDescription: %s\nFlags: #%d\n", adapter->name, adapter->description, adapter->flags);
			// get adapter addresses
			for (pcap_addr* address = adapter->addresses; address; address = address->next) {
				char* address_buffer;
				size_t buffer_size = 0;
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
					current_adapter.push_back(address_buffer);
				}
			}
			// Set global adapter_data
			m_adapter_data.push_back(current_adapter);
		}
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int Sniffer::openAdapter(unsigned int adapter_id, int max_packet_size, int promiscuous_mode) {
		if (!checkForAdapterId(adapter_id)) { RETURN_CODE(RC(ADAPTER_NOT_FOUND)); } // specified adapter not found
		// open handle
		char error_buffer[PCAP_ERRBUF_SIZE]; // pcap error buffer

		// resize handles vector as needed
		if (m_adapter_handles.size() <= adapter_id) {
			m_adapter_handles.resize(adapter_id + 1);
		}
		// open handle for live capturing
		m_adapter_handles[adapter_id] = pcap_open_live(m_adapter_data[adapter_id][0], max_packet_size, promiscuous_mode, 0, error_buffer);
		if (!m_adapter_handles[adapter_id]) {
			fprintf(stderr, "Failed to open handle on network device #%d (%s)\n%s\n", adapter_id, m_adapter_data[adapter_id][0], error_buffer);
			RETURN_CODE(RC(ERROR_OPENING_HANDLE));
		}
		DEBUG(1, "Capturing started on device #%d\n", adapter_id);
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int Sniffer::openAdapter(char* adapter_name, int max_packet_size, int promiscuous_mode) {
		return openAdapter(adapterId(adapter_name, ADAPTER_NAME), max_packet_size, promiscuous_mode);
	}

	int Sniffer::closeAdapter(char* adapter_name) {
		return closeAdapter(adapterId(adapter_name, ADAPTER_NAME));
	}

	int Sniffer::closeAdapter(unsigned int adapter_id) {
		if (m_adapter_handles.size() > adapter_id && m_adapter_handles[adapter_id]) {
			pcap_close(m_adapter_handles[adapter_id]);
			m_adapter_handles[adapter_id] = NULL;
			RETURN_CODE(RC(NORMAL_EXECUTION));
		}
		// Nothing to close
		RETURN_CODE(RC(CLOSE_ON_UNOPENED_HANDLE));
	}

	int Sniffer::freeAdapters() {
		if (m_adapter_raw_data == NULL) {
			// nothing to free
			RETURN_CODE(RC(NORMAL_EXECUTION));
		}
		for (pcap_t* handle : m_adapter_handles) {
			if (handle) { pcap_close(handle); }
		}
		int i;
		for (std::vector<char*> adapter : m_adapter_data) {
			// i >= 2 := Address strings
			for (i = 2; i < adapter.size(); i++) {
				// Free allocated memory
				free(adapter[i]);
			}
		}
		// clear vector content
		m_adapter_data.clear();
		m_adapter_handles.clear();
		// free adapters
		pcap_freealldevs( m_adapter_raw_data );
		// purge old values
		m_adapter_raw_data	= NULL;
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int Sniffer::applyFilter(unsigned int adapter_id, char* filter) {
		
		if (!checkForAdapterId(adapter_id)) { return -1; } // specified adapter not found
		const char*			filter_string = (filter ? filter : ""); // If given filter is NULL, replace with empty(/ANY) filter
		struct bpf_program	filter_compiled;
		pcap_t*				handle = NULL;
		if (m_adapter_handles.size() > adapter_id) {
			handle = m_adapter_handles[adapter_id];
		}
		if (!handle) {
			fprintf(stderr, "Error: applyFilter() called on unopened adapter.\n");
			RETURN_CODE(RC(ACCESS_ON_UNOPENED_HANDLE));
		}
		if (pcap_compile(handle, &filter_compiled, filter_string, 1, 0) < 0) {
			fprintf(stderr, "Error: Failed to compile given filter.\n");
			RETURN_CODE(RC(ERROR_COMPILING_FILTER));
		}
		if (pcap_setfilter(handle, &filter_compiled) < 0) {
			fprintf(stderr, "Error: Failed to apply the given filter.\n");
			RETURN_CODE(RC(ERROR_APPLYING_FILTER));
		}
		DEBUG(1, "Filter successfully applied.\nFilter: %s\n", filter_string);
		RETURN_CODE(RC(NORMAL_EXECUTION));
	}

	int Sniffer::applyFilter(char* adapter_name, char* filter) {
		return applyFilter(adapterId(adapter_name, ADAPTER_NAME), filter);
	}

	int Sniffer::removeFilter(char* adapter_name) {
		return applyFilter(adapter_name, NULL);
	}

	int Sniffer::removeFilter(unsigned int adapter_id) {
		return applyFilter(adapter_id, NULL);
	}

	Sniffer::PcapPacket Sniffer::retrievePacket(unsigned int adapter_id) {
		PcapPacket packet = { NULL, NULL };
		if (!checkForAdapterId(adapter_id)) {
			// specified adapter not found
			RETURN_VALUE(RC(ADAPTER_NOT_FOUND), packet);
		}
		/*
			struct pcap_pkthdr:
				struct timeval	ts; - system dependent size (either 32 or 64bit)
				bpf_u_int32		caplen;
				bpf_u_int32		len;
		*/
		struct pcap_pkthdr 	packet_header;
		const u_char*		packet_data;
		pcap_t*				handle = NULL;
		if (m_adapter_handles.size() > adapter_id) {
			handle = m_adapter_handles[adapter_id];
		}
		if (!handle) {
			fprintf(stderr, "Error: retrievePacket() called on unopened adapter.\n");
			RETURN_VALUE(RC(ACCESS_ON_UNOPENED_HANDLE), packet);
		}
		/* Note: 
			packet_data can contain NULL if
			1. an error occured
			2. no packets were read from live capture (read timeout)
			3. no packet passed the filter
			Quote from winpcap doc: "Unfortunately, there is no way to determine whether an error occured or not."
			( http://www.winpcap.org/docs/docs_412/html/group__wpcapfunc.html#gadf60257f650aaf869671e0a163611fc3 )
		*/
		packet_data  = pcap_next(handle, &packet_header); // returns pointer to the packet data (pcap header not included)
		packet.header = packet_header;
		DEBUG(3, "Packet data: %u - length: %d\n", (packet_data ? reinterpret_cast<const unsigned int*>(packet_data) : 0), packet_header.len);
		if (packet_data) {
			packet.payload = packet_data;
			RETURN_VALUE(RC(NORMAL_EXECUTION), packet);
		} else {
			RETURN_VALUE(RC(EMPTY_PACKET_DATA), packet);
		}
	}

	Sniffer::PcapPacket Sniffer::retrievePacket(char* adapter_name) {
		return retrievePacket(adapterId(adapter_name, ADAPTER_NAME));
	}

	std::vector<bool> Sniffer::retrievePacketAsVector(unsigned int adapter_id) {
		std::vector<bool> bitVector;
		PcapPacket packet			= retrievePacket(adapter_id);
		if (packet.payload == NULL) {
			RETURN_VALUE(RC(EMPTY_PACKET_DATA), bitVector);
		}
		unsigned int packet_size	= packet.header.len;
		const unsigned char* it		= packet.payload;
		bitVector					= std::vector<bool>(packet_size * sizeof(u_char));
		unsigned int i, j;
		for (i = 0; i < packet_size; i++) {
			for (j = 0; j < 8; j++) {
				bitVector.push_back((*it & (1 << j)) != 0);
			}
			it++;
		}
		RETURN_VALUE(RC(NORMAL_EXECUTION), bitVector);
	}

	std::vector<bool> Sniffer::retrievePacketAsVector(char* adapter_name) {
		return retrievePacketAsVector(adapterId(adapter_name, ADAPTER_NAME));
	}

	char* Sniffer::ipToString(struct sockaddr* socket_address, char* buffer, size_t buffer_length) {
		if (!socket_address) { return NULL; }
		// sockaddr_storage requires Windows XP, Windows Server 2003 or later
		socklen_t address_length = sizeof(struct sockaddr_storage);
		DEBUG(2, "ipToString()\nBuffer length: %d; Address Length: %d\n", static_cast<unsigned int>(buffer_length), static_cast<unsigned int>(address_length));
		
		getnameinfo(socket_address, address_length, buffer, buffer_length, NULL, 0, NI_NUMERICHOST);
		DEBUG(2, "Address from getnameinfo: %s\n\n", (buffer) ? buffer : "NULL");
		return (buffer);
	}

	std::vector<int> Sniffer::lastReturnCodes() {
		std::vector<int> returnCodes = std::vector<int>(20);
		for (int rc : m_last_return_codes) {
			returnCodes.push_back(rc);
		}
		return returnCodes;
	}
} 