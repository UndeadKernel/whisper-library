//author: Jan Simon Bunten

#ifndef TCP_PACKAGE
#define TCP_PACKAGE
#define maxUInt = 65000;
#define uint unsigned int

#include "common.hpp"
#include <vector>

namespace whisperLibrary {

	class /*WHISPERAPI*/ TcpPackage { 

	public:
		TcpPackage();
		/* Locations of the header informations:
		 * 0-15 Source port, 16-31 Destination port
		 * 32-63 Sequence number
		 * 63-95 Acknowledgement number
		 * 96-99 Data offset
		 * 100-102 Reserved
		 * 103 NS, 104 CWR, 105 ECE, 106 URG, 107 ACK
		 * 108 PSH, 109 RST, 110 SYN, 111 FIN, 112-127 Window Size
		 * 128-143 checksum, 144-159 Urgent Pointer
		 * Options field:
		 * 0-319 depending on data-offset
		 * 0-7 option kind, 8-15 option length
		 * (option-length) option data
		 */

		uint get_sourcePort(); // 16bit
		uint get_destPort(); // 16 bit

		uint get_sequenceNumber(); // 32 bit
		uint get_acknowlageNumber(); // 32 bit

		uint get_dataOffset(); // 4 bit
		uint get_reserved(); // 3 bit

		uint get_flags(); // 9 bit
		bool get_ns();
		bool get_cwr();
		bool get_ece();
		bool get_urg();
		bool get_ack();
		bool get_psh();
		bool get_rst();
		bool get_syn();
		bool get_fin();

		uint get_windowSize();
		uint get_checksum();
		std::vector<uint> get_options();
		std::vector<bool>* get_package();
		std::vector<uint> get_data();

		void set_sourcePort(uint val); // 16bit
		void set_destPort(uint val); // 16 bit

		void set_sequenceNumber(uint val); // 32 bit
		void set_acknowlageNumber(uint val); // 32 bit

		void set_dataOffset(uint val); // 4 bit
		void set_reserved(uint val); // 3 bit

		void set_flags(uint val); // 9 bit
		void set_ns(bool val);
		void set_cwr(bool val);
		void set_ece(bool val);
		void set_urg(bool val);
		void set_ack(bool val);
		void set_psh(bool val);
		void set_rst(bool val);
		void set_syn(bool val);
		void set_fin(bool val);

		void set_windowSize(uint val);
		void set_checksum(uint val);
		void set_options(uint* val, uint size);
		
		void set_data(uint* val, uint size);


	private:
		std::vector<bool> header;
		std::vector<bool> options;
		std::vector<bool> data;
		bool* intToBoolArray(uint val);
		uint vectorToUInt(int start, int end, std::vector<bool> &set);
		void uIntToVector(int start, int end, std::vector<bool> &set, uint val);
	};
}
#endif
