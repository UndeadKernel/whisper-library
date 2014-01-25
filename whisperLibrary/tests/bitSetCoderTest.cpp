#include "bitSetCoder.hpp"
#include <boost\test\unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>

using namespace std;

struct BitSetCoderFixture {
	string msg;
	string msg_decoded;
};

BOOST_FIXTURE_TEST_SUITE(bitSetCoder, BitSetCoderFixture)

BOOST_AUTO_TEST_CASE(encode_simple_message) {
	whisper_library::BitSetCoder<6> coder;
	msg = "AB";

	vector<bitset<6>> vec = coder.encodeMessage(msg);
	msg_decoded = coder.decodeMessage(vec);

	BOOST_CHECK_EQUAL(msg, msg_decoded);
}

BOOST_AUTO_TEST_CASE(encode_empty_message) {
	whisper_library::BitSetCoder<6> coder;
	msg = "";

	vector<bitset<6>> vec = coder.encodeMessage(msg);
	msg_decoded = coder.decodeMessage(vec);

	BOOST_CHECK_EQUAL(msg, msg_decoded);
}

BOOST_AUTO_TEST_CASE(encode_10_bit) {
	whisper_library::BitSetCoder<10> coder;
	msg = "";

	vector<bitset<10>> vec = coder.encodeMessage(msg);
	msg_decoded = coder.decodeMessage(vec);

	BOOST_CHECK_EQUAL(msg, msg_decoded);
}

BOOST_AUTO_TEST_SUITE_END()