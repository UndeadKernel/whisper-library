// 	Author: Martin Oehler

#ifndef BITSET_CODER
#define BITSET_CODER

#include "common.hpp"
#include <string>
#include <bitset>
#include <vector>
#include <iostream>

using namespace std;
namespace whisper_library {
	/*
	BitSetCoder converts the message, that we want to send, into bit-blocks and reassembles them to a message.
	BLOCK_LENGTH sets the length of these bit-blocks.
	To use this class, call the function 'encodeMessage' with the message as a string or 'decodeMessage' with the message-blocks in a vector.
	*/
	template <size_t BLOCK_LENGTH> class BitSetCoder {
	public:
		/*
		Returns the size of a message block (number of bits).
		*/
		size_t blockLength() {
			return BLOCK_LENGTH;
		}
		/*
		encodeMessage takes a string and splits it into parts of 6 bit.
		These are returned as a vector of bitsets in order. If you can't divide the bit count by 6, the last bits are filled up with 0's.
		The bitsets contain the binary representation of ASCI-encoded letters.
		*/
		vector<bitset<BLOCK_LENGTH>> encodeMessage(string message) {
			string bit_sequence = "";
			vector<bitset<BLOCK_LENGTH>> ret_vector;

			// do nothing if the message is empty
			if (message.length() == 0) {
				return ret_vector;
			}
			// convert chars to binary
			for (unsigned int i = 0; i < message.length(); ++i) {
				bitset<8> bit(message[i]);
				bit_sequence += bit.to_string();
			}
			//split binary
			unsigned int i = 0;
			while (i + BLOCK_LENGTH <= bit_sequence.length()) {
				bitset<BLOCK_LENGTH> bit(bit_sequence.substr(i, BLOCK_LENGTH));
				ret_vector.push_back(bit);
				i += BLOCK_LENGTH;
			}
			// build trailing zeroes if bit_sequence length didn't match the bit block size
			if (i != bit_sequence.length()) {
				string zeroes = "";
				for (unsigned int j = 0; j < BLOCK_LENGTH - (bit_sequence.length() - i); j++) {
					zeroes += "0";
				}
				// add trailing zeroes
				if (zeroes != "") {
					ret_vector.push_back(bitset<BLOCK_LENGTH>(bit_sequence.substr(i, bit_sequence.length() - i) + zeroes));
				}
			}
			return ret_vector;
		}

		/*
		decodeMessage takes 6-bit blocks and converts them to characters returned as a string.
		The bitsets contain the binary representation of ASCI-encoded letters.
		*/
		string decodeMessage(vector<bitset<BLOCK_LENGTH>> vector) {
			string bit_sequence = "";
			// append bits to string
			for (unsigned int i = 0; i < vector.size(); ++i) {
				bit_sequence += vector[i].to_string();
			}

			// convert binary string to chars
			string message = "";
			for (unsigned int i = 0; i + 8 <= bit_sequence.length(); i += 8) {
				bitset<8> bitset(bit_sequence.substr(i, 8));
				char c = static_cast<char>(bitset.to_ulong());
				message.push_back(c);
			}
			return message;
		}
	};
}

#endif //BITSET_CODER