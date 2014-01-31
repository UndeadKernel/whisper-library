// 	Author: Martin Oehler

#ifndef MORSE_CODER
#define MORSE_CODER

#include <vector>

using namespace std;

namespace whisper_library {
	class MorseCoder {
	public:
		vector<float> encodeMessage(string message);

		string decodeMessage(vector<float> delays);

	private:

	};
}

#endif //MORSE_CODER