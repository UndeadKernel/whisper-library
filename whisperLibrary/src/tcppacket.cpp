#include "tcppacket.hpp"
#include <iostream>
#include <math.h>

using namespace std;

namespace whisper_library {

TcpPacket::TcpPacket(){
    m_header.resize(160);
}

TcpPacket::TcpPacket(uint inSourcePort, 
						uint inDestPort, 
						ulong inSequenceNumber,
						ulong inAckNumber,
						bitset<4> inDataOffset,
						uint inWindowSize,
						vector<bool> inOptions){
		
    m_header.resize(160);
        
    setSourcePort(inSourcePort);
    setDestPort(inDestPort);
    setSequenceNumber(inSequenceNumber);
    setAcknowlageNumber(inAckNumber);
    setDataOffset(inDataOffset);
    setWindowSize(inWindowSize);
    setOptions(inOptions);
}
	
/* 	
	Locations of the header informations:
	0-15 Source port, 16-31 Destination port
	32-63 Sequence number
	64-95 Acknowledgement number
	96-99 Data offset
	100-102 Reserved
	103 NS, 104 CWR, 105 ECE, 106 URG, 107 ACK
	108 PSH, 109 RST, 110 SYN, 111 FIN, 112-127 Window Size
	128-143 checksum, 144-159 Urgent Pointer
	Options field:
	0-319 depending on data-offset
	0-7 option kind, 8-15 option length
	(option-length) option data
*/
	 
// header: bits 0-15
uint TcpPacket::sourcePort() const{
    return vectorToULong(0, 15, m_header);
}
// header: bits 16-31
uint TcpPacket::destPort() const{
    return vectorToULong(16, 31, m_header);
} 
// header: bits 32-63  
ulong TcpPacket::sequenceNumber() const{
    return vectorToULong(32, 63, m_header);
}
// header: bits 64-95 
ulong TcpPacket::acknowlageNumber() const{
    return vectorToULong(64, 95, m_header);
}
// header: bits 96-99  
bitset<4> TcpPacket::dataOffset() const{
    bitset<4> ret;
    for (int i = 96; i <= 99; i++){
		ret.set(i-96, m_header[i]);
	}
    return ret;
}
// header: bits 100-102
bitset<3> TcpPacket::reserved() const{
    bitset<3> ret;
    for (int i = 100; i <= 102; i++){
		ret.set(i-100, m_header[i]);
	}
    return ret;
}
// header: bits 103-111  
bitset<9> TcpPacket::flags() const{
    bitset<9> ret;
    for (int i = 103; i <= 111; i++){
		ret.set(i-103, m_header[i]);
	}
    return ret;
}
// header: bit 103
bool TcpPacket::nonceSumFlag() const{
    return m_header[103];
}
// header: bit 104
bool TcpPacket::congestionWindowReducedFlag() const{
    return m_header[104];
}
// header: bit 105
bool TcpPacket::ecnEchoFlag() const{
    return m_header[105];
}
// header: bit 106
bool TcpPacket::urgentFlag() const{
    return m_header[106];
}
// header: bit 107
bool TcpPacket::acknowledementFlag() const{
    return m_header[107];
}
// header: bit 108
bool TcpPacket::pushFlag() const{
    return m_header[108];
}
// header: bit 109
bool TcpPacket::resetFlag() const{
    return m_header[109];
}
// header: bit 110
bool TcpPacket::synchronisationFlag() const{
    return m_header[110];
}
// header: bit 111
bool TcpPacket::finishFlag() const{
    return m_header[111];
}
// header: bits 112-127
uint TcpPacket::windowSize() const{
    return vectorToULong(112, 127, m_header);
}
// header: bits 128-143
uint TcpPacket::checksum() const{
    return vectorToULong(128, 143, m_header);
}
// header: bits 128-143
uint TcpPacket::urgentPointer() const{
    return vectorToULong(128, 143, m_header);
}
// options: bits 0-320
vector<bool> TcpPacket::options() const{
    vector<bool> ret (m_options);
    return ret;
}
// packet
vector<bool> TcpPacket::packet() const{
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
vector<bool> TcpPacket::data() const{
    vector<bool> ret (m_data);
    return ret;
}

// header: bits 0-15  
void TcpPacket::setSourcePort(uint val){
    uIntToVector(0,15,m_header,val);
}
// header: bits 16-31
void TcpPacket::setDestPort(uint val){
    uIntToVector(16,31,m_header,val);
}
// header: bits 32-63  
void TcpPacket::setSequenceNumber(ulong val){
    uIntToVector(32,63,m_header,val);
}
// header: bits 64-95
void TcpPacket::setAcknowlageNumber(ulong val){
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
void TcpPacket::setFlags(bitset<9> val){
    for (int i = 103; i <= 111; i++){
		m_header[i] = val[i-103];
	}
}
// header: bit 103
void TcpPacket::setNonceSumFlag(bool val){
    m_header[103] = val;
}
// header: bit 104
void TcpPacket::setCongestionWindowReducedFlag(bool val){
    m_header[104] = val;
}
// header: bit 105
void TcpPacket::setEcnEchoFlag(bool val){
    m_header[105] = val;
}
// header: bit 106
void TcpPacket::setUrgentFlag(bool val){
    m_header[106] = val;
}
// header: bit 107
void TcpPacket::setAcknowledgementFlag(bool val){
    m_header[107] = val;
}
// header: bit 108
void TcpPacket::setPushFlag(bool val){
    m_header[108] = val;
}
// header: bit 109
void TcpPacket::setResetFlag(bool val){
    m_header[109] = val;
}
// header: bit 110
void TcpPacket::setSynchronisationFlag(bool val){
    m_header[110] = val;
}
// header: bit 111
void TcpPacket::setFinishFlag(bool val){
    m_header[111] = val;
}
// header: bits 112-127
void TcpPacket::setWindowSize(uint val){
    uIntToVector(112,127,m_header,val);
}
// header: bits 128-143
void TcpPacket::setChecksum(uint val){
    uIntToVector(128,143,m_header,val);
}
// header: bits 144-159
void TcpPacket::setUrgentPointer(uint val){
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
// packet
void TcpPacket::setPacket(vector<bool> val){
	for (int i = 0; i < 160; i++){
		m_header[i] = val[i];
	}
	int offset = dataOffset().to_ulong();
	int lengthopt = (offset*8) - 160;
	m_options.empty();
	for (int i = 160; i < lengthopt+160; i++){
		m_options.push_back(val[i]);
	} 
	m_data.empty();
	for (int i = lengthopt + 160; i < val.size(); i++){
		m_data.push_back(val[i]);
	}
}
    
void TcpPacket::calculateChecksum(ulong sourceIp, ulong destIp, uint reservedBits, uint protocol){
    vector<bool> sum;
	/* 
		split the tcp packet into 16bit values and add all of them up
		using the one complement addition
	*/
	vector<vector<bool> > split;
	split = splitHeaderTo16Bit();
	for (int i = 0; i < split.size(); i++){
		sum = oneComplementAdd(sum, split[i]);
	}
	// split the source IP into 16bit values and add them to the sum
	vector<bool> temp (intToBoolVector(sourceIp));
	reverse(temp.begin(), temp.end());
	split = split32BitVector(temp);
	for (int i = 0; i < split.size(); i++){
		sum = oneComplementAdd(sum, split[i]);
	}
	// split the destination IP into 16bit values and add them to the sum
	temp = intToBoolVector(destIp);
	reverse(temp.begin(), temp.end());
	split = split32BitVector(temp);
	for (int i = 0; i < split.size(); i++){
		sum = oneComplementAdd(sum, split[i]);
	}
	/*
		combine the remaining values for the IP headers reserved bits, 
		the protocol used and the size of the tcp packet in bytes into a
		new vector, split it into 16bit values and add them to the sum
	*/ 
	vector<bool> combine (intToBoolVector(reservedBits));
	reverse(combine.begin(), combine.end());
	combine = trimBigEndianVector(combine, 8);
	temp = intToBoolVector(protocol);
	reverse(temp.begin(), temp.end());
	temp = trimBigEndianVector(temp, 8);
	for (int i = 0; i < temp.size(); i++){
		combine.push_back(temp[i]);
	}
    vector<bool> temp2(intToBoolVector((m_header.size() + m_options.size() + m_data.size()) / 8));
    reverse(temp2.begin(), temp2.end());
    temp2 = (trimBigEndianVector(temp2, 16));
    for (int i = 0; i < temp2.size(); i++){
        combine.push_back(temp2[i]);
	}
	split = split32BitVector(combine);
	for (int i = 0; i < split.size(); i++){
		sum = oneComplementAdd(sum, split[i]);
	}
	// if the resulting sum vector exeeds 16 bits add the exeeding bits as carry to the sum
    if (sum.size()>16){
        vector<bool> remainder;
        for (int i = 0; i < 16; i++){
            remainder.push_back(sum.back());
            sum.pop_back();
        }
        sum = oneComplementAdd(sum,remainder);
    }
    // compute the one complement of the sum and store it as the new checksum
    sum.flip();
    setChecksum(vectorToULong(0, sum.size()-1, sum));
}
	
    
ulong TcpPacket::vectorToULong(int start, int end, const vector<bool> &vec) const{
	int ret = 0;
	for (int i = start; i <= end; i++){
		if 	(vec[i])
            ret += 1 << (end -i);
	}
	return ret;
}
     
template <class T> void TcpPacket::uIntToVector(int start, int end, const vector<bool> &vec, T val){
    vector<bool> ins (intToBoolVector(val));
	for (int i = end; i >= start; i--){
		vec[i] = ins[end-i];
	}
}
    
template <class T> vector<bool> TcpPacket::intToBoolVector(T val){
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
    // convert the vectors to little endian to make the addition easier
    reverse(vec1.begin(), vec1.end());
    reverse(vec2.begin(), vec2.end());
    vector<bool> result;
	bool carry = false;
	int sum = 0;
	//compute the bitwise one complement addition of the two vectors
    for (int i = 0; i < 16; i++){
		sum = 0;
		// computing carry and value of the sum at position i
		if (carry)
			sum = sum + 1;
		if ((i < vec1.size()) && vec1[i])
			sum = sum + 1;
		if ((i < vec2.size()) && vec2[i])
			sum = sum + 1;
		if (sum == 0){
            result.push_back(false);
            carry = false;
		}
		if (sum == 1){
            result.push_back(true);
            carry = false;
		}
		if (sum == 2){
            result.push_back(false);
            carry = true;
		}
		if (sum == 3){
            result.push_back(true);
            carry = true;
		}
	}
    reverse(result.begin(), result.end());
    // if there is an overflow add the remaining carry to the sum
    if (carry){
        return oneComplementAdd(vector<bool>(1,true), result);
	}
        
    return result;
}
	
vector<vector<bool> > TcpPacket::split32BitVector(vector<bool> vec){
	vector<bool> vec1;
	vector<bool> vec2;
	vector< vector<bool> > result;
	for (int i = 0; i < 16; i++){
		vec1.push_back(vec[i]);
		vec2.push_back(vec[i+16]);
	}
	result.push_back(vec1);
	result.push_back(vec2);
	return result;
}
	
vector<vector<bool> > TcpPacket::splitHeaderTo16Bit(){
	vector< vector<bool> > result;
	for (int i = 0; i < (m_header.size()/16); i++){
        vector<bool> temp;
		for (int j = 0; j < 16; j++){
			temp.push_back(m_header[(i*16)+j]);
		}
		result.push_back(temp);
	}
	if (m_options.empty() == false){
		for (int i = 0; i < (m_options.size()/16); i++){
			vector<bool> temp;
			for (int j = 0; j < 16; j++){
				temp.push_back(m_options[(i*16)+j]);
			}
			result.push_back(temp);
		}
	}
	if (m_data.empty() == false){
		for (int i = 0; i < (m_data.size()/16); i++){
			vector<bool> temp;
			for (int j = 0; j < 16; j++){
				temp.push_back(m_data[(i*16)+j]);
			}
			result.push_back(temp);
		}
	}
	return result;
}
	
vector<bool> TcpPacket::trimBigEndianVector(vector<bool> vec, int size){
	vector<bool> ret;
	for (int i = vec.size()-size; i < vec.size(); i++){
		ret.push_back(vec[i]);
	}
	return ret;
}
} //whisper_library
