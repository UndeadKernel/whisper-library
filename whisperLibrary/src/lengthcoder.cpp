/*	<lengthcoder.cpp>
	Copyright(C) 2014	Jan Simon Bunten
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

#include "lengthcoder.hpp"
#include "bitSetCoder.hpp"

namespace whisper_library {

	std::vector<unsigned int> LengthCoder::encodeMessage(std::string message){
		std::vector<unsigned int> result;
		std::vector<bitset<4>> parts;
		BitSetCoder<4> coder;
		parts = coder.encodeMessage(message); //split into parts of 4 bit
		for (int i = 0; i < parts.size(); i++){
			result.push_back(parts[i].to_ulong()+8+m_baseLength); //calculate packet length
		}
		return result;
	}

	std::string LengthCoder::decodeMessage(std::vector<unsigned int> lengths){
		std::string result;
		std::vector<bitset<4>> parts;
		for (int i = 0; i < lengths.size(); i++){ //get packetlengths as bits
			int length = lengths[i] - 8 - m_baseLength;
			parts.push_back(bitset<4>((length<0||length>15)?0:length));
		}
		BitSetCoder<4> coder;
		result = coder.decodeMessage(parts); //decode bitsets
		return result;
	}

}