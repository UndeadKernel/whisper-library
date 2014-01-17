//author: Jan Simon Bunten

#ifndef TCP_PACKET
#define TCP_PACKET

#include "common.hpp"
#include <vector>
#include <bitset>

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

        int sourcePort(); // 16bit
        int destPort(); // 16 bit

        int sequenceNumber(); // 32 bit
        int acknowlageNumber(); // 32 bit

        std::bitset<4> dataOffset(); // 4 bit
        std::bitset<3> reserved(); // 3 bit

        std::bitset<9> flags(); // 9 bit
        bool ns();
        bool cwr();
        bool ece();
        bool urg();
        bool ack();
        bool psh();
        bool rst();
        bool syn();
        bool fin();

        int windowSize();
        int checksum();
        int urgentPointer();
        std::vector<bool> options();
        std::vector<bool> packet();
        std::vector<bool> data();

		void setSourcePort(int val); // 16bit
		void setDestPort(int val); // 16 bit

		void setSequenceNumber(int val); // 32 bit
		void setAcknowlageNumber(int val); // 32 bit

		void setDataOffset(std::bitset<4> val); // 4 bit
		void setReserved(std::bitset<3> val); // 3 bit

        void setFlags(std::bitset<9> &val); // 9 bit
		void setNs(bool val);
		void setCwr(bool val);
		void setEce(bool val);
		void setUrg(bool val);
		void setAck(bool val);
		void setPsh(bool val);
		void setRst(bool val);
		void setSyn(bool val);
		void setFin(bool val);

		void setWindowSize(int val);
		void setChecksum(int val);
        void setUrgentPointer(int val);
        void setOptions(std::vector<bool> val);
		
        void setData(std::vector<bool> val);


	private:
        std::vector<bool> m_header;
        std::vector<bool> m_options;
        std::vector<bool> m_data;
        int vectorToUInt(int start, int end, std::vector<bool> &vec);
        void uIntToVector(int start, int end, std::vector<bool> &vec, int val);
        std::vector<bool> intToBoolVector(int val);
	};
}
#endif
