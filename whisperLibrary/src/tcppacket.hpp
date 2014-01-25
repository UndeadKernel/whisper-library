//author: Jan Simon Bunten

#ifndef TCP_PACKET
#define TCP_PACKET

#include "../include/whisperLibrary/common.hpp"
#include <vector>
#include <bitset>
#include <algorithm>

using namespace std;
typedef unsigned int uint;
typedef unsigned long ulong;

namespace whisper_library {

// This class represents a tcp packet. All fields are accessable.
class TcpPacket {

public:
    // The empty constructor creates an empty tcp packet.
    TcpPacket();
	
    /* 
		This constructor is used to fill in some of the important fields
		beforehand.
    */
    TcpPacket(uint inSourcePort, 
				uint inDestPort, 
				ulong inSequenceNumber,
				ulong inAckNumber,
				bitset<4> inDataOffset,
				uint inWindowSize,
				vector<bool> inOptions);

    // start of the getters
    uint sourcePort() const;
    uint destPort() const;
    ulong sequenceNumber() const;
    ulong acknowlageNumber() const;
    bitset<4> dataOffset() const;
    bitset<3> reserved() const;
    bitset<9> flags() const;
    bool nonceSumFlag() const;
    bool congestionWindowReducedFlag() const;
    bool ecnEchoFlag() const;
    bool urgentFlag() const;
    bool acknowledementFlag() const;
    bool pushFlag() const;
    bool resetFlag() const;
    bool synchronisationFlag() const;
    bool finishFlag() const;
    uint windowSize() const;
    uint checksum() const;
    uint urgentPointer() const;
    vector<bool> options() const;
    vector<bool> packet() const;
    vector<bool> data() const;

    // start of the setters
	void setSourcePort(uint val);
	void setDestPort(uint val);
	void setSequenceNumber(ulong val);
	void setAcknowlageNumber(ulong val);
	void setDataOffset(bitset<4> val);
	void setReserved(bitset<3> val);
    void setFlags(bitset<9> val);
    void setNonceSumFlag(bool val);
    void setCongestionWindowReducedFlag(bool val);
    void setEcnEchoFlag(bool val);
    void setUrgentFlag(bool val);
    void setAcknowledgementFlag(bool val);
    void setPushFlag(bool val);
    void setResetFlag(bool val);
    void setSynchronisationFlag(bool val);
    void setFinishFlag(bool val);
	void setWindowSize(uint val);
	void setChecksum(uint val);
    void setUrgentPointer(uint val);
    void setOptions(vector<bool> val);
    void setData(vector<bool> val);
    void setPacket(vector<bool> val);
        
    // other public functions
        
    /* 
		This function is used to calculate the tcp checksum.
		To do this it needs some additional information of the IP header as parameters.
    */
    void calculateChecksum(ulong sourceIp, ulong destIp, uint reservedBits, uint protocol);

private: 
    /* 
		This function is used to convert a vector into an unsigned integer value.
		The parameters are start and end bits of the number in the given vector
		and the vector the number is stored in.
    */
    ulong vectorToULong(int start, int end, const vector<bool> &vec) const;

    /* 
		This function is used to insert an unsigned integer value into a given boolean vector.
		The parameters needed are the start and end of where to put the integer
		into the vector, the vector itself and the value to be inserted.
    */
    template <class T> void uIntToVector(int start, int end, vector<bool> &vec, T val);

    // This function is used to convert a given integer value into a boolean vector.
    template <class T> vector<bool> intToBoolVector(T val);

    /* 
		This function uses the one complement addition to add two boolean vectors.
		The input boolean vectors max size is 16. 
		The returned sum will have a max size of 16.
    */ 
    vector<bool> oneComplementAdd(vector<bool> vec1, vector<bool> vec2);

    // This function splits a 32 bit boolean vector into two 16 bit vectors
	vector<vector<bool> > split32BitVector(vector<bool> vec);

    // This function splits the complete packet into a vector of 16bit boolean vectors.
	vector<vector<bool> > splitHeaderTo16Bit();

    // This function trims a big endian to a fixed size
	vector<bool> trimBigEndianVector(vector<bool> vec, int size);


	vector<bool> m_header;
    vector<bool> m_options;
    vector<bool> m_data;	
};
}
#endif
