#include <whisperLibrary\bitSetDecoder.hpp>
#include <boost/test/unit_test.hpp>

#include<string>
#include<vector>
#include<bitset>
#include <iostream>

using namespace std;

struct Fixture {
	Fixture(){
		dec = whisper_library::BitSetDecoder();
	}
	~Fixture(){
	}
	vector<bitset<6>> test_vec;
	string expected_output = "";
	string output = "";
	whisper_library::BitSetDecoder dec;
};

BOOST_FIXTURE_TEST_SUITE(bitSetDecoder, Fixture)


BOOST_AUTO_TEST_CASE(decode_simple_message) {
	bitset<6> bit("010000");
	cout << "test" << endl;
	test_vec.push_back(bit);
	cout << "test2" << endl;
	test_vec.push_back(bitset<6>("010100"));
	test_vec.push_back(bitset<6>("001000"));

	expected_output = "AB";

	output = dec.decodeMessage(test_vec);
	BOOST_CHECK_EQUAL(output, expected_output);
}/*


BOOST_AUTO_TEST_CASE(decode_empty_message) {

	expected_output = "";

	output = dec.decodeMessage(test_vec);
	BOOST_CHECK_EQUAL(output, expected_output);
} */

BOOST_AUTO_TEST_SUITE_END()