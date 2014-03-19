/*	<lengthcoder.hpp>
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

#ifndef LENGTH_CODER
#define LENGTH_CODER

#include <vector>
#include <string>

namespace whisper_library {
	/** \brief Converts between messages and packet lengths (used by PacketLengthCovertChannel)

		A LengthCoder can be used to encode messages into udp packet lengths and decode
		udp packet lengths into a message. A base length/offset for the packet lengths
		can be set to avoid packets with no data payload (length = 8).

		The LengthCoder uses the BitSetCoder to split the message into parts
		\see BitSetCoder
	*/
	class LengthCoder {
	public:
		/** \brief Constructor of LengthCoder

			Creates an LengthCoder and sets the base length, which is added to every
			packet length
			\param baselength length offset for every packet
		*/
		LengthCoder(int baselength) :m_baselength(baselength){};
		/** \brief encodes a message to packet lengths
			
			The given message gets translated to packet lengths variing from baseLength
			to baseLength + 15.
			\param message the message that gets encoded
			\returns an vector containing udp packet Length in the range of baseLength to
			baseLength + 15.
		*/
		std::vector<unsigned int> encodeMessage(std::string message);
		/** \brief decodes packet lengths
			
			The lengths from the given vector get decoded into a string. The lengths
			should be between baseLength and baseLength + 15
			\param lengths a vector containing received udp packet lengths
			\returns a string with the decoded message
		*/
		std::string decodeMessage(std::vector<unsigned int> lengths);
	private:
		int m_baselength;///< gets added/substracted from the packet lengths
	};
}

#endif // LENGTH_CODER
