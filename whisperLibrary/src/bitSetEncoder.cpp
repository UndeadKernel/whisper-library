// Descriptions in header.

#include <bitSetEncoder.hpp>

using namespace std;

namespace whisperLibrary {

	vector<bitset<6>> BitSetEncoder::encodeMessage(string msg) {
		string bitstream = "";
		vector<bitset<6>> vec;

		// do nothing if the message is empty
		if (msg.length() == 0) {
			return vec;
		}

		// convert chars to binary
		for (unsigned int i = 0; i < msg.length(); ++i) {
			bitset<8> bit(msg[i]);
			bitstream += bit.to_string();
		}

		//split binary
		unsigned int i = 0;
		while (i + 6 < bitstream.length()) {
			bitset<6> bit(bitstream.substr(i, 6));
			vec.push_back(bit);

			i += 6;
		}

		// build trailing zeroes
		string zeros = "";
		for (unsigned int i = 0; i < bitstream.length() - i; i++) {
			zeros += "0";
		}

		// add trailing zeroes
		vec.push_back(bitset<6>(bitstream.substr(i, bitstream.length() - 1 - i) + zeros));
		return vec;
	}

}