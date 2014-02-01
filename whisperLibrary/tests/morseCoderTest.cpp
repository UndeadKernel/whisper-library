#include "morseCoder.hpp"
#include <boost/test/unit_test.hpp>

using namespace std;

struct MorseCoderFixture {
	string msg;
	string msg_decoded;
};

BOOST_FIXTURE_TEST_SUITE(morseCoder, MorseCoderFixture)

BOOST_AUTO_TEST_CASE(morse_encode_simple_message) {
	whisper_library::MorseCoder morsecoder;
	vector<bool> encoding = morsecoder.encodeLetter('b');

	for (vector<bool>::iterator it = encoding.begin(); it != encoding.end(); it++) {
		cout << (*it);
	}
	cout << endl;
}

BOOST_AUTO_TEST_SUITE_END()