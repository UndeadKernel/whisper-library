#include <whisperLibrary/bitSetDecoder.hpp>
#include <boost/test/unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>

using namespace std;

typedef bitset<6> bitset6;

struct BitSetDecoderFixture {
	BitSetDecoderFixture(){
		dec = whisper_library::BitSetDecoder();
		expected_output = "";
		output = "";
	}
	~BitSetDecoderFixture(){
	}
	vector<bitset6> test_vec;
	string expected_output;
	string output;
	whisper_library::BitSetDecoder dec;
};

BOOST_FIXTURE_TEST_SUITE(bitSetDecoder, BitSetDecoderFixture)


BOOST_AUTO_TEST_CASE(decode_simple_message) {
	test_vec.push_back(bitset<6>(string("010000")));
	test_vec.push_back(bitset<6>(string("010100")));
	test_vec.push_back(bitset<6>(string("001000")));

	expected_output = "AB";

	output = dec.decodeMessage(test_vec);
	BOOST_CHECK_EQUAL(output, expected_output);
}


BOOST_AUTO_TEST_CASE(decode_empty_message) {
	expected_output = "";

	output = dec.decodeMessage(test_vec);
	BOOST_CHECK_EQUAL(output, expected_output);
} 

BOOST_AUTO_TEST_SUITE_END()
