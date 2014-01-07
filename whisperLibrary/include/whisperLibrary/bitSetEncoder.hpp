// 	Author: Martin Oehler

#ifndef BITSET_ENCODER
#define BITSET_ENCODER

#include "common.hpp"
#include <string>
#include <bitset>
#include <vector>

using namespace std;
namespace whisper_library {

/*
	BitSetEncoder converts the message, that we want to send, into 6bit-blocks.
	To do this, call the function 'encodeMessage' with the message as a string.
*/
class WHISPERAPI BitSetEncoder {

public:
	/*
		encodeMessage takes a string and splits it into parts of 6 bit.
		These are returned as a vector of bitsets in order. If you can't divide the bit count by 6, the last bits are filled up with 0's.
		The bitsets contain the binary representation of ASCI-encoded letters.
	*/
	vector<bitset<6>> encodeMessage(string message);
private:
	vector<bitset<6>> encodeMessageHelper(string message);
};
}

#endif //BITSET_ENCODER