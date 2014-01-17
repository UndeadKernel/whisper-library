#include "tcppacket.hpp"
#include <iostream>


using namespace std;
namespace whisper_library {
    TcpPacket::TcpPacket(){
        m_header.resize(159);
        m_options.resize(319);
	}
	TcpPacket::TcpPacket(int inSourcePort, 
					int inDestPort, 
					int inSequenceNumber,
					int inAckNumber,
					bitset<4> inDataOffset,
					int inWindowSize,
					vector<bool> inOptions){
		
		m_header.resize(159);
        m_options.resize(319);
        
        setSourcePort(inSourcePort);
        setDestPort(inDestPort);
        setSequenceNumber(inSequenceNumber);
        setAcknowlageNumber(inAckNumber);
        setDataOffset(inDataOffset);
        setWindowSize(inWindowSize);
        setOptions(inOptions);
	}
	
	/* Locations of the header informations:
		 * 0-15 Source port, 16-31 Destination port
		 * 32-63 Sequence number
		 * 64-95 Acknowledgement number
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
	 
	// header: bits 0-15
    int TcpPacket::sourcePort(){
        return vectorToUInt(0, 15, m_header);
	}
	// header: bits 16-31
    int TcpPacket::destPort(){
        return vectorToUInt(16, 31, m_header);
	} 
	// header: bits 32-63  
    int TcpPacket::sequenceNumber(){
        return vectorToUInt(32, 63, m_header);
    }
	// header: bits 64-95 
    int TcpPacket::acknowlageNumber(){
        return vectorToUInt(64, 95, m_header);
    }
	// header: bits 96-99  
    bitset<4> TcpPacket::dataOffset(){
        bitset<4> ret;
        for (int i = 96; i <= 99; i++){
			ret.set(i-96, m_header[i]);
		}
        return ret;
    }
	// header: bits 100-102
    bitset<3> TcpPacket::reserved(){
        bitset<3> ret;
        for (int i = 100; i <= 102; i++){
			ret.set(i-100, m_header[i]);
		}
        return ret;
    }
	// header: bits 103-111  
    bitset<9> TcpPacket::flags(){
        bitset<9> ret;
        for (int i = 103; i <= 111; i++){
			ret.set(i-103, m_header[i]);
		}
        return ret;
    }
	// header: bit 103
    bool TcpPacket::ns(){
        return m_header.at(103);
    }
	// header: bit 104
    bool TcpPacket::cwr(){
        return m_header.at(104);
    }
	// header: bit 105
    bool TcpPacket::ece(){
        return m_header.at(105);
    }
	// header: bit 106
    bool TcpPacket::urg(){
        return m_header.at(106);
    }
	// header: bit 107
    bool TcpPacket::ack(){
        return m_header.at(107);
    }
	// header: bit 108
    bool TcpPacket::psh(){
        return m_header.at(108);
    }
	// header: bit 109
    bool TcpPacket::rst(){
        return m_header.at(109);
    }
	// header: bit 110
    bool TcpPacket::syn(){
        return m_header.at(110);
    }
	// header: bit 111
    bool TcpPacket::fin(){
        return m_header.at(111);
    }
	// header: bits 112-127
    int TcpPacket::windowSize(){
        return vectorToUInt(112, 127, m_header);
    }
	// header: bits 128-143
    int TcpPacket::checksum(){
        return vectorToUInt(128, 143, m_header);
    }
    // header: bits 128-143
    int TcpPacket::urgentPointer(){
        return vectorToUInt(128, 143, m_header);
    }
	// options: bits 0-320
    vector<bool> TcpPacket::options(){
        vector<bool> ret (m_options);
        return ret;
    }
    // packet
    vector<bool> TcpPacket::packet(){
        vector<bool> ret (m_header);
        for (int i = 0; i < m_options.size(); i++){
            ret.push_back(m_options[i]);
        }
        for (int i = 0; i < m_data.size(); i++){
            ret.push_back(m_data[i]);
        }
        return ret;
    }

    // data
    vector<bool> TcpPacket::data(){
        vector<bool> ret (m_data);
        return ret;
    }

	// header: bits 0-15  
    void TcpPacket::setSourcePort(int val){
        uIntToVector(0,15,m_header,val);
    }
	// header: bits 16-31
    void TcpPacket::setDestPort(int val){
        uIntToVector(16,31,m_header,val);
    }
	// header: bits 32-63  
    void TcpPacket::setSequenceNumber(int val){
        uIntToVector(32,63,m_header,val);
    }
	// header: bits 64-95
    void TcpPacket::setAcknowlageNumber(int val){
        uIntToVector(64,95,m_header,val);
    }
	// header: bits 96-99  
    void TcpPacket::setDataOffset(bitset<4> val){
        for (int i = 96; i <= 99; i++){
			m_header[i] = val[i-96];
		}
    }
	// header: bits 100-102
    void TcpPacket::setReserved(bitset<3> val){
        for (int i = 100; i <= 102; i++){
			m_header[i] = val[i-100];
		}
    }
	// header: bits 103-111  
    void TcpPacket::setFlags(bitset<9> &val){
        for (int i = 103; i <= 111; i++){
			m_header[i] = val[i-103];
		}
    }
	// header: bit 103
    void TcpPacket::setNs(bool val){
        m_header[103] = val;
    }
	// header: bit 104
    void TcpPacket::setCwr(bool val){
        m_header[104] = val;
    }
	// header: bit 105
    void TcpPacket::setEce(bool val){
        m_header[105] = val;
    }
	// header: bit 106
    void TcpPacket::setUrg(bool val){
        m_header[106] = val;
    }
	// header: bit 107
    void TcpPacket::setAck(bool val){
        m_header[107] = val;
    }
	// header: bit 108
    void TcpPacket::setPsh(bool val){
        m_header[108] = val;
    }
	// header: bit 109
    void TcpPacket::setRst(bool val){
        m_header[109] = val;
    }
	// header: bit 110
    void TcpPacket::setSyn(bool val){
        m_header[110] = val;
    }
	// header: bit 111
    void TcpPacket::setFin(bool val){
        m_header[111] = val;
    }
	// header: bits 112-127
    void TcpPacket::setWindowSize(int val){
        uIntToVector(112,127,m_header,val);
    }
	// header: bits 128-143
    void TcpPacket::setChecksum(int val){
        uIntToVector(128,143,m_header,val);
    }
    // header: bits 144-159
    void TcpPacket::setUrgentPointer(int val){
        uIntToVector(144,159,m_header,val);
    }
	// options
    void TcpPacket::setOptions(vector<bool> val){
        m_options = vector<bool>(val);
    }
    // data
    void TcpPacket::setData(vector<bool> val){
        m_data = vector<bool>(val);
    }
    
    void TcpPacket::calculateChecksum(int sourceIp, int destIp, int reservedBits, int protocol){
		vector<bool> sum (destPort());
		vector<vector<bool> > split;
		split = splitHeaderTo16Bit();
		for (int i = 0; i < split.size(); i++){
			oneComplementAdd(sum, split[i]);
		}
		split = split32BitVector(intToBoolVector(sourceIp));
		for (int i = 0; i < split.size(); i++){
			oneComplementAdd(sum, split[i]);
		}
		split = split32BitVector(intToBoolVector(destIp));
		for (int i = 0; i < split.size(); i++){
			oneComplementAdd(sum, split[i]);
		}
		vector<bool> combine (intToBoolVector(reservedBits));
		vector<bool> temp (intToBoolVector(protocol));
		for (int i = 0; i < temp.size(); i++){
			combine.push_back(temp[i]);
		}
		vector<bool> temp2 (intToBoolVector((m_header.size() + m_options.size() + m_data.size()) / 8));
		for (int i = 0; i < temp.size(); i++){
			combine.push_back(temp[i]);
		}
		split = split32BitVector(combine);
		for (int i = 0; i < split.size(); i++){
			oneComplementAdd(sum, split[i]);
		}
	}
	
    
    int TcpPacket::vectorToUInt(int start, int end, vector<bool> &vec){
    int ret = 0;
		for (int i = start; i <= end; i++){
			if 	(vec.at(i))
				ret += 1 << (i - start);
		}
		return ret;
	}
     
    void TcpPacket::uIntToVector(int start, int end, vector<bool> &vec, int val){
    	vector<bool> ins(intToBoolVector(val));
		for (int i = start; i <= end; i++){
			vec.at(i) = ins[i-start];
		}
    }
    
	vector<bool> TcpPacket::intToBoolVector(int val){
        vector<bool> ret;
        for(int i = 0; i<32; i++){
            if (val % 2 == 1){
                ret.push_back(true);
            }
            else
				ret.push_back(false);
            val = val / 2;
        }
        return ret;
    }
    
    vector<bool> TcpPacket::oneComplementAdd(vector<bool> vec1, vector<bool> vec2){
		
	}
	
	vector<vector<bool> > TcpPacket::split32BitVector(vector<bool> vec){
		
	}
	
	vector<vector<bool> > TcpPacket::splitHeaderTo16Bit(){
		
	}
}
