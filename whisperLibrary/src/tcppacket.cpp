/*	<tcppacket.cpp>
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
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/
#include "tcppacket.hpp"
#include <iostream>
#include <math.h>

using namespace std;

namespace whisper_library {

TcpPacket::TcpPacket(){
    m_header.resize(160);
}

TcpPacket::TcpPacket(	uint inSourcePort, 
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
    setAcknowlegeNumber(inAckNumber);
    setDataOffset(inDataOffset);
    setWindowSize(inWindowSize);
    setOptions(inOptions);
}
	
	TcpPacket::TcpPacket(vector<bool> packet){
		setPacket(packet);
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
ulong TcpPacket::acknowlegeNumber() const{
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
bool TcpPacket::acknowledgementFlag() const{
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
    for (uint i = 0; i < m_options.size(); i++){
        ret.push_back(m_options[i]);
    }
    for (uint i = 0; i < m_data.size(); i++){
        ret.push_back(m_data[i]);
    }
    return ret;
}

// data
vector<bool> TcpPacket::data() const{
    return m_data;
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
void TcpPacket::setAcknowlegeNumber(ulong val){
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
void TcpPacket::setData(string data) {
	vector<bool> binary;
	for (unsigned int i = 0; i < data.length(); i++) {
		char byte = data[i];
		for (unsigned int j = 0; j < 8; j++) {
			unsigned char bit = byte >> (7 - j);
			bit = bit & 0x01;
			if (bit == 0) {
				binary.push_back(false);
			}
			else {
				binary.push_back(true);
			}
		}
	}
	setData(binary);
}
// packet
void TcpPacket::setPacket(vector<bool> val){
	if (val.size() >= 160) {
		m_header.clear();
        for (int i = 0; i < 160; i++){
            m_header.push_back(val[i]);
		}
        uint offset = vectorToULong(96, 99, m_header);
		uint option_length = (offset*32) - 160;
		if (offset > 5){
			m_options.clear();
			for (uint i = 160; i < option_length+160; i++){
				m_options.push_back(val[i]);
			} 
		}
		m_data.clear();
		for (uint i = option_length + 160; i < val.size(); i++){
			m_data.push_back(val[i]);
		}
	}
}
    
void TcpPacket::calculateChecksum(ulong sourceIp, ulong destIp, uint reservedBits, uint protocol){
    vector<bool> sum;
	/* 
		split the tcp packet into 16bit values and add them to 
		a vector.
	*/
	vector<vector<bool> > vector_of_words;
	vector_of_words = splitHeaderTo16Bit();
	
	// split the source IP into 16bit values and add them to the sum
	vector<bool> temporary_vector (intToBoolVector(sourceIp, 32));
	reverse(temporary_vector.begin(), temporary_vector.end());
	vector< vector<bool> > words_to_append = split32BitVector(temporary_vector);
	for (vector< vector<bool> >::iterator it = words_to_append.begin(); it != words_to_append.end(); it++){
		vector_of_words.push_back(*it);
	}
	
	// split the destination IP into 16bit values and add them to the sum
	temporary_vector = intToBoolVector(destIp, 32);
	reverse(temporary_vector.begin(), temporary_vector.end());
	words_to_append = split32BitVector(temporary_vector);
	for (vector< vector<bool> >::iterator it = words_to_append.begin(); it != words_to_append.end(); it++){
		vector_of_words.push_back(*it);
	} 
	
	// combine reserved bits and protocoll
	vector<bool> combine_vectors (intToBoolVector(reservedBits, 8));
	reverse(combine_vectors.begin(), combine_vectors.end());
	combine_vectors = trimBigEndianVector(combine_vectors, 8);
	temporary_vector = intToBoolVector(protocol, 8);
	reverse(temporary_vector.begin(), temporary_vector.end());
	temporary_vector = trimBigEndianVector(temporary_vector, 8);
	for (vector<bool>::iterator it = temporary_vector.begin(); it != temporary_vector.end(); it++){
		combine_vectors.push_back(*it);
	}
	vector_of_words.push_back(combine_vectors);
	
	// calculate tcp packet size in bytes and add it to the vector
    temporary_vector = (intToBoolVector(((m_header.size() + m_options.size() + m_data.size()) / 8), 16));
    reverse(temporary_vector.begin(), temporary_vector.end());
    temporary_vector = (trimBigEndianVector(temporary_vector, 16));
	vector_of_words.push_back(temporary_vector);
	
	// ones complement add all the values to the sum
	for (vector< vector<bool> >::iterator it = vector_of_words.begin(); it != vector_of_words.end(); it++){
		sum = oneComplementAdd(sum, *it);
	}

    // compute the one complement of the sum and store it as the new checksum
    sum.flip();
    setChecksum(vectorToULong(0, (sum.size()-1), sum));
}
	
    
ulong TcpPacket::vectorToULong(uint start, uint end, const vector<bool> &vec) const{
	ulong ret = 0;
	if (start <= end && end < vec.size()){
		for (uint i = start; i <= end; i++){
			if 	(vec[i])
				ret += (1 << (end-i));
		}
	}
	return ret;
}
     
template <class T> void TcpPacket::uIntToVector(uint start, uint end, vector<bool> &vec, T val){
	if (start <= end && end < vec.size()){
        vector<bool> insert (intToBoolVector(val, (end - start + 1)));
        reverse(insert.begin(), insert.end());
        for (uint i = start; i <= end; i++){
            vec[i] = insert[i-start];
		}
	}
}
    
template <class T> vector<bool> TcpPacket::intToBoolVector(T val, uint size){
    vector<bool> ret;
    if (val < 0){
		ret = vector<bool>(size,false);
		return ret;
	}
    for(uint i = 0; i<size; i++){
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
    vec1.resize(16, false);
    vec2.resize(16, false);
    vector<bool> result;
	bool carry = false;
	int sum = 0;
	//compute the bitwise one complement addition of the two vectors
    for (uint i = 0; i < 16; i++){
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
	for (vector<bool>::iterator it = vec.begin(); it != vec.end(); it++){
		if (vec1.size() < 16)
			vec1.push_back(*it);
		else
			vec2.push_back(*it);
	}
	result.push_back(vec1);
	result.push_back(vec2);
	return result;
}
	
vector<vector<bool> > TcpPacket::splitHeaderTo16Bit(){
	vector< vector<bool> > result;
	for (uint i = 0; i < (m_header.size()/16); i++){
        vector<bool> temp;
		for (int j = 0; j < 16; j++){
			temp.push_back(m_header[(i*16)+j]);
		}
		result.push_back(temp);
	}
	if (m_options.empty() == false){
		for (uint i = 0; i < (m_options.size()/16); i++){
			vector<bool> temp;
			for (int j = 0; j < 16; j++){
				temp.push_back(m_options[(i*16)+j]);
			}
			result.push_back(temp);
		}
	}
	if (m_data.empty() == false){
		for (uint i = 0; i < (m_data.size()/16); i++){
			vector<bool> temp;
			for (int j = 0; j < 16; j++){
				temp.push_back(m_data[(i*16)+j]);
			}
			result.push_back(temp);
		}
	}
	return result;
}
	
vector<bool> TcpPacket::trimBigEndianVector(vector<bool> vec, uint size){
	vector<bool> ret;
	for (uint i = vec.size()-size; i < vec.size(); i++){
		ret.push_back(vec[i]);
	}
	if (ret.size() < size)
		vec.resize(size);
	
	return ret;
}

} //whisper_library
