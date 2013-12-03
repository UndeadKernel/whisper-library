// author: Martin Oehler
// Class that converts the message, that we want to send, to a format, that our covert channel can use.

#ifndef ENCODER
#define ENCODER


#include "common.hpp"

#include <string>
#include <bitset>
#include <vector>

using namespace std;

namespace whisperLibrary {

class WHISPERAPI Encoder {

public:
	Encoder();
	vector<bitset<6>> encodeMessage(string msg);

private:

};

}



#endif