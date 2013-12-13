#define BOOST_TEST_MODULE whisperLibraryTestSuite
#include <boost/test/included/unit_test.hpp>


struct GFixture {
    GFixture() : test_log("test.log")  {
		boost::unit_test::unit_test_log.set_stream( test_log );
		test_log << "WhisperLibrary Tests:" << std::endl;
	}
    ~GFixture(){
		test_log << "Tests executed" << std::endl;
		boost::unit_test::unit_test_log.set_stream( std::cout );
	}

    std::ofstream test_log;
};

BOOST_GLOBAL_FIXTURE(GFixture);