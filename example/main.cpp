
#include <iostream>
#include <whisperLibrary/sniffer.hpp>
#include <ctime>
#include <time.h>

using namespace std;

int main(int argc, char* argv[]){
	
	whisper_library::Sniffer sniffer = whisper_library::Sniffer();
	if (sniffer.adapterCount() == 0) {
		return -1;
	}
	unsigned char packet_buffer[100];
	int buffer_length = 100;

	/* Supposing to be on ethernet, set mac destination to 1:1:1:1:1:1 */
	packet_buffer[0] = 1;
	packet_buffer[1] = 1;
	packet_buffer[2] = 1;
	packet_buffer[3] = 1;
	packet_buffer[4] = 1;
	packet_buffer[5] = 1;

	/* set mac source to 2:2:2:2:2:2 */
	packet_buffer[6] = 2;
	packet_buffer[7] = 2;
	packet_buffer[8] = 2;
	packet_buffer[9] = 2;
	packet_buffer[10] = 2;
	packet_buffer[11] = 2;

	/* Fill the rest of the packet */
	for (int i = 12; i < 100; i++)
	{
		packet_buffer[i] = i % 256;
	}
	sniffer.openAdapter(0, sniffer.DEFAULT_MAXPACKETSIZE, true, 1);

	// Wireshark display filter: eth.addr == 1:1:1:1:1:1
	sniffer.sendPacket(0, packet_buffer, buffer_length);

	return 0;
	/*
	int id;
	vector<char*> names = sniffer.adapterNames();
	for (char* name : names) {
		printf("###Name: %s\n", name);
		id = sniffer.adapterId(name, sniffer.ADAPTER_NAME);
		for (char* addr : sniffer.adapterAddresses(id)) {
			printf("Address: %s\n", addr);
		}
		sniffer.openAdapter(id, sniffer.DEFAULT_MAXPACKETSIZE, true, 1);
		// http://www.winpcap.org/docs/docs_40_2/html/group__language.html
		sniffer.applyFilter(id, "");

		/*
		struct pcap_pkthdr:
		struct timeval	ts; - system dependent size (either 32 or 64bit)
		bpf_u_int32		caplen;
		bpf_u_int32		len;
		
		for (int k = 0; k < 10000; k++) {
			whisper_library::Sniffer::PcapPacket packet = sniffer.retrievePacket(id);
			if (packet.payload == NULL) { continue; }
			unsigned long p_ts, s_ts;
			unsigned long s_uts, p_uts;
			p_ts	= packet.header.ts.tv_sec;
			p_uts	= static_cast<unsigned long>(packet.header.ts.tv_usec);

			FILETIME ft;
			GetSystemTimeAsFileTime(&ft);
			unsigned long long tt = ft.dwHighDateTime;
			tt <<= 32;
			tt |= ft.dwLowDateTime;
			// CONVERT TO POSIX TS
			tt /= 10;
			tt -= 11644473600000000ULL; // EPOCH IN MICROSECONDS
			s_ts  = static_cast<long>(tt / 1000000UL);
			s_uts = static_cast<long>(tt % 1000000UL);
			fprintf(stdout, "sys: %lu\npac: %lu\n", s_uts, p_uts);
			if (s_ts - p_ts > 1 || s_uts - p_uts > 5000) { // packages with more than 1s or more than 5ms delay
				fprintf(stdout, "Timestamp - Packet: %lu + %llu ; Current: %lu + %lu\n", p_ts, p_uts, s_ts, s_uts);
			}
		}
		sniffer.removeFilter(id);
		sniffer.closeAdapter(id);
	}
	sniffer.freeAdapters();
	*/
}

