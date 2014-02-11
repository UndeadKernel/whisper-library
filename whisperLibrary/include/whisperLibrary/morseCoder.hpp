// 	Author: Martin Oehler

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>
#include <boost/bimap.hpp>

using namespace std;

typedef boost::bimap<char, string> morse_map;
typedef morse_map::value_type morse;

namespace whisper_library {
	/*
		MorseCoder converts a message into time intervals using morse and decodes morse back to a message.
		To do so, call 'encodeMessage' with the message or 'decodeMessage' with time intervals. 
		Use 'checkString' to check if the given message contains characters, that are not supported by morse.
	*/
	class MorseCoder {
	public:
		/*
			Constructor
			delay_short is used to encode a short signal (in milliseconds)
			delay_long is used to encode a long signal (in milliseconds)
			delay_letter is used to encode the end of a letter (in milliseconds)
			delay_space is used to encode space between words (in milliseconds)
		*/
		MorseCoder(unsigned int delay_short, unsigned int delay_long, unsigned int delay_letter, unsigned int delay_space);

		/*
			encodeMessage takes a message as a string and converts it into a sequence of delays using morse.
			Unsupported characters are encoded as '#'.
		*/
		vector<unsigned int> encodeMessage(string message);
		/*
			decodeMessage takes a sequence of delays representing morse and converts them back to a message.
			The delays have to be exactly the ones set in the constructor.
		*/
		string decodeMessage(vector<unsigned int> delays);

		/*
			Checks the given string for characters, that are not supported by morse.
			If none were found, the returned vector is empty. Otherwise it contains unsupported characters.
		*/
		vector<char> checkString(string message);

	private:
		/*
			Encodes a single character to morse. 
			The encoded character is returned as a vector of bool in which 0 (false) means short and 1 (true) means long.
		*/
		vector<bool> encodeLetter(char letter);

		/*
			Decodes morse for a single character
			The passed vector of bool has to contain the encoded letter in which 0 (false) means short and 1 (true) means long.
		*/
		char decodeLetter(vector<bool> morse_code);

		// Represents a short signal (in milliseconds)
		unsigned int m_delay_short;
		// Represents a long signal (in milliseconds)
		unsigned int m_delay_long;
		// Represents a short pause (in milliseconds)
		unsigned int m_delay_letter;
		// Represents a long pause (in milliseconds)
		unsigned int m_delay_space;

		// Contains the mapping between characters and morse.
		morse_map m_morse_map;
	};
}

#endif //MORSE_CODER