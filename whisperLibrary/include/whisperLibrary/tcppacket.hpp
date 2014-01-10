//author: Jan Simon Bunten

#ifndef TCP_PACKET
#define TCP_PACKET
#define uint unsigned int

#include "common.hpp"
#include <vector>

namespace whisperLibrary {

    class /*WHISPERAPI*/ TcpPacket {

	public:
        TcpPacket();
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

        uint sourcePort(); // 16bit
        uint destPort(); // 16 bit

        uint sequenceNumber(); // 32 bit
        uint acknowlageNumber(); // 32 bit

        uint dataOffset(); // 4 bit
        uint reserved(); // 3 bit

        std::vector<bool>* flags(); // 9 bit
        bool ns();
        bool cwr();
        bool ece();
        bool urg();
        bool ack();
        bool psh();
        bool rst();
        bool syn();
        bool fin();

        uint windowSize();
        uint checksum();
        std::vector<bool>* options();
        std::vector<bool>* packet();
        std::vector<bool>* data();

		void set_sourcePort(uint val); // 16bit
		void set_destPort(uint val); // 16 bit

		void set_sequenceNumber(uint val); // 32 bit
		void set_acknowlageNumber(uint val); // 32 bit

		void set_dataOffset(uint val); // 4 bit
		void set_reserved(uint val); // 3 bit

        void set_flags(std::vector<bool> &val); // 9 bit
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
        void set_options(std::vector<bool> &val);
		
        void set_data(std::vector<bool> &val);


	private:
        std::vector<bool> _header;
        std::vector<bool> _options;
        std::vector<bool> _data;
        uint vectorToUInt(int start, int end, std::vector<bool> &set);
        void uIntToVector(int start, int end, std::vector<bool> &set, uint val);
        bool* intToBoolArray(uint val);
	};
}
#endif
