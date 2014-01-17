//author: Jan Simon Bunten

#ifndef TCP_PACKET
#define TCP_PACKET

#include "common.hpp"
#include <vector>
#include <bitset>


using namespace std;
namespace whisper_library {

    class /*WHISPERAPI*/ TcpPacket {

	public:
        TcpPacket();
        TcpPacket(int inSourcePort, 
					int inDestPort, 
					int inSequenceNumber,
					int inAckNumber,
					bitset<4> inDataOffset,
					int inWindowSize,
					vector<bool> inOptions);
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

        bitset<4> dataOffset(); // 4 bit
        bitset<3> reserved(); // 3 bit

        bitset<9> flags(); // 9 bit
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
        vector<bool> options();
        vector<bool> packet();
        vector<bool> data();

		void setSourcePort(int val); // 16bit
		void setDestPort(int val); // 16 bit

		void setSequenceNumber(int val); // 32 bit
		void setAcknowlageNumber(int val); // 32 bit

		void setDataOffset(bitset<4> val); // 4 bit
		void setReserved(bitset<3> val); // 3 bit

        void setFlags(bitset<9> &val); // 9 bit
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
        void setOptions(vector<bool> val);
		
        void setData(vector<bool> val);
        
        void calculateChecksum(int sourceIp, int destIp, int reservedBits, int protocol);


	private:
        vector<bool> m_header;
        vector<bool> m_options;
        vector<bool> m_data;
        int vectorToUInt(int start, int end, vector<bool> &vec);
        void uIntToVector(int start, int end, vector<bool> &vec, int val);
        vector<bool> intToBoolVector(int val);
        vector<bool> oneComplementAdd(vector<bool> vec1, vector<bool> vec2);
		vector<vector<bool> > split32BitVector(vector<bool> vec);
		vector<vector<bool> > splitHeaderTo16Bit();
	};
}
#endif
