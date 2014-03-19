/*	<morseCoder.hpp>
	Copyright(C) 2014   Jan Simon Bunten
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

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>
#include <boost/bimap.hpp>

using namespace std;

typedef boost::bimap<char, string> morse_map;
typedef morse_map::value_type morse;

namespace whisper_library {
	/**	\brief converts messages and timing intervals back and forth (used by TimingCovertChannel)

		MorseCoder converts a message into time intervals using morse and decodes morse back to a message.
		To do so, call 'encodeMessage' with the message or 'decodeMessage' with time intervals. 
		Use 'checkString' to check if the given message contains characters, that are not supported by morse.
	*/
	class MorseCoder {
	public:
		/** \brief Constructor


			\param delay_short is used to encode a short signal (in milliseconds)
			\param delay_long is used to encode a long signal (in milliseconds)
			\param delay_letter is used to encode the end of a letter (in milliseconds)
			\param delay_space is used to encode space between words (in milliseconds)
		*/
		MorseCoder(unsigned int delay_short, unsigned int delay_long, unsigned int delay_letter, unsigned int delay_space);

		/**
			encodeMessage converts a message into a sequence of delays using morse.
			Unsupported characters are encoded as '#'.

			\param message the message as string
		*/
		vector<unsigned int> encodeMessage(string message);
		/**
			decodeMessage takes a sequence of delays representing morse and converts them back to a message.
			The delays have to be exactly the ones set in the constructor.

			\param delays a vector containing delays
		*/
		string decodeMessage(vector<unsigned int> delays);

		/**
			Checks the given string for characters, that are not supported by morse.
			If none were found, the returned vector is empty. Otherwise it contains unsupported characters.
			\param message the string that is checked
			\returns all unsupported characters in the message
		*/
		vector<char> checkString(string message);

	private:
		/**
			Encodes a single character to morse. 
			\param letter the character to decode
			\returns the morse of the letter, 0 (false) means short and 1 (true) means long
		*/
		vector<bool> encodeLetter(char letter);

		/**
			Decodes morse for a single character
			The passed vector of bool has to contain the encoded letter in which 0 (false) means short and 1 (true) means long.
			\param morse_code the morse of a single letter
			\returns the letter as char
		*/
		char decodeLetter(vector<bool> morse_code);
		
		unsigned int m_delay_short;///< Represents a short signal (in milliseconds)		
		unsigned int m_delay_long;///< Represents a long signal (in milliseconds)		
		unsigned int m_delay_letter;///< Represents a short pause (in milliseconds)		
		unsigned int m_delay_space;///< Represents a long pause (in milliseconds)
		morse_map m_morse_map;///< Contains the mapping between characters and morse.
	};
}

#endif //MORSE_CODER