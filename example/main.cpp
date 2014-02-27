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

	int id;
	vector<char*> names = sniffer.adapterNames();
	for (char* name : names) {
		printf("###Name: %s\n", name);
		id = sniffer.adapterId(name, sniffer.ADAPTER_NAME);
		for (char* addr : sniffer.adapterAddresses(id)) {
			printf("Address: %s\n", addr);
		}
		sniffer.openAdapter(id, sniffer.DEFAULT_MAXPACKETSIZE, sniffer.PROMISCUOUS_MODE_ON);
		// http://www.winpcap.org/docs/docs_40_2/html/group__language.html
		sniffer.applyFilter(id, "");

		/*
		struct pcap_pkthdr:
		struct timeval	ts; - system dependent size (either 32 or 64bit)
		bpf_u_int32		caplen;
		bpf_u_int32		len;
		*/
		for (int k = 0; k < 10000; k++) {
			whisper_library::Sniffer::PcapPacket packet = sniffer.retrievePacket(id);
			long p_ts, p_uts, s_ts, s_uts;
			p_ts	= packet.header.ts.tv_sec;
			p_uts	= packet.header.ts.tv_usec;
			s_ts	= static_cast<long>(time(0));
			s_uts	= 
			if (s_ts - p_ts > 1) {
				fprintf(stdout, "Timestamp - Packet: %ld ; Current: %ld\n", p_ts, s_ts);
			}
		}
		sniffer.removeFilter(id);
		sniffer.closeAdapter(id);
	}
	sniffer.freeAdapters();
}
