#include <lengthcoder.hpp>
#include <boost/test/unit_test.hpp>

struct LengthCoderFixture {
	LengthCoderFixture(){
		m_coder = new whisper_library::LengthCoder(10);
	}
	~LengthCoderFixture(){
		delete m_coder;
	}
	whisper_library::LengthCoder* m_coder;
};

BOOST_FIXTURE_TEST_SUITE(LengthCoderTest, LengthCoderFixture)

BOOST_AUTO_TEST_CASE(EncodeEmptyString){
	std::string msg = "";
	std::vector<unsigned int> lengths = m_coder->encodeMessage(msg);
	std::string result = m_coder->decodeMessage(lengths);
	BOOST_CHECK_EQUAL(msg, result);
}

BOOST_AUTO_TEST_CASE(EncodeShortString){
	std::string msg = "test";
	std::vector<unsigned int> lengths = m_coder->encodeMessage(msg);
	std::string result = m_coder->decodeMessage(lengths);
	BOOST_CHECK_EQUAL(msg, result);
}

BOOST_AUTO_TEST_CASE(EncodeLongString){
	std::string msg = "Long Test to see if the LengthCoder can handle this";
	std::vector<unsigned int> lengths = m_coder->encodeMessage(msg);
	std::string result = m_coder->decodeMessage(lengths);
	BOOST_CHECK_EQUAL(msg, result);
}

BOOST_AUTO_TEST_SUITE_END()