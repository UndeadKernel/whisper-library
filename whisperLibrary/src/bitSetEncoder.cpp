// author: Martin Oehler
// Class that converts the message, that we want to send, to a format, that our covert channel can use.

#include <bitSetEncoder.hpp>

using namespace std;

namespace whisperLibrary {


	BitSetEncoder::BitSetEncoder() {
	
	}
	
	vector<bitset<6>> BitSetEncoder::encodeMessage(string msg) {
		string bitstream = "";

		for (unsigned int i = 0; i < msg.length(); ++i) {
			bitset<8> bit(msg[i]);
			bitstream += bit.to_string();
		}

		vector<bitset<6>> vec;
		unsigned int i = 0;
		while (i + 6 < bitstream.length()) {
			bitset<6> bit(bitstream.substr(i, 6));
			vec.push_back(bit);

			i += 6;
		}

		string zeros = "";
		for (unsigned int i = 0; i < bitstream.length() - i; i++) {
			zeros += "0";
		}

		vec.push_back(bitset<6>(bitstream.substr(i, bitstream.length() - 1 - i) + zeros));
		return vec;
	}

}