// Class that converts the message, that we want to send, to a format, that our covert channel can use.
// Author: Martin Oehler

#ifndef BITSET_ENCODER
#define BITSET_ENCODER

#include "common.hpp"

#include <string>
#include <bitset>
#include <vector>

using namespace std;

namespace whisperLibrary {

class WHISPERAPI BitSetEncoder {

  public:
	// encodeMessage takes a string and splits it into parts of 6 bit.
	// These are returned as a vector in order (0-n). If you can't divide the bit count by 6, the last bits are filled up with 0's.
	// INPUT:
	//			string msg -> The message that we want to split
	// OUTPUT:
	//			vector<bitset<6>> -> A vector containing bitsets. The bitsets contain the binary representation of ASCI-encoded letters.
	//								 To reconvert the message, you have to append the bitsets in order (0-n). 
	vector<bitset<6>> encodeMessage(string msg);

  private:

};

}



#endif