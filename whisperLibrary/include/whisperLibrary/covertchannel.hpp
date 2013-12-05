#ifndef COVERT_CHANNEL
#define COVERT_CHANNEL

#include <encoder.hpp>

namespace whisperLibrary {

class CovertChannel {

public:
	CovertChannel();

private:
	Encoder encoder;
};

}


#endif