/*	<bitSetCoder.hpp>
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

#ifndef BITSET_CODER
#define BITSET_CODER

#include "common.hpp"
#include <string>
#include <bitset>
#include <vector>

using namespace std;
namespace whisper_library {
	/**
	\brief BitSetCoder converts the message, that we want to send, into bit-blocks and reassembles them to a message.
	
	To use this class, call the function 'encodeMessage' with the message as a string or 'decodeMessage' with the message-blocks in a vector.
	\class BitSetCoder bitSetCoder.hpp
	\tparam BLOCK_LENGTH sets the length of the bit-blocks.
	*/
	template <size_t BLOCK_LENGTH> class BitSetCoder {
	public:
		/**
			Returns the size of a message block.
			\return the size of a message block as number of bits
		*/
		size_t blockLength() {
			return BLOCK_LENGTH;
		}
		/**
		\brief encodeMessage takes a string and splits it into parts.
		
		These are returned as a vector of bitsets in order. If you can't divide the bit count by BLOCK_LENGTH, the last bits are filled up with 0's.
		The bitsets contain the binary representation of ASCI-encoded letters.
		\param message the message to be encoded
		\return the message as a vector of bitsets with the size of BLOCK_LENGTH
		*/
		vector<bitset<BLOCK_LENGTH> > encodeMessage(string message) {
			string bit_sequence = "";
			vector<bitset<BLOCK_LENGTH> > ret_vector;
			unsigned int i;

			// do nothing if the message is empty
			if (message.length() == 0) {
				return ret_vector;
			}
			// convert chars to binary
			for (i = 0; i < message.length(); ++i) {
				bitset<8> bit(message[i]);
				bit_sequence += bit.to_string();
			}
			//split binary
			i = 0;
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

		/**
		decodeMessage takes bit blocks and converts them to characters returned as a string.
		
		\param vector a vector of bitsets with the bitsets containing the binary representation of ASCII-encoded letters.
		\return the message as string
		*/
		string decodeMessage(vector<bitset<BLOCK_LENGTH> > vector) {
			string bit_sequence = "";
			unsigned int i;
			// append bits to string
			for (i = 0; i < vector.size(); ++i) {
				bit_sequence += vector[i].to_string();
			}

			// convert binary string to chars
			string message = "";
			for (i = 0; i + 8 <= bit_sequence.length(); i += 8) {
				bitset<8> bitset(bit_sequence.substr(i, 8));
				char c = static_cast<char>(bitset.to_ulong());
				message.push_back(c);
			}
			return message;
		}
	};
}

#endif //BITSET_CODER
