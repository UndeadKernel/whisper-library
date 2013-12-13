#ifndef ENCODER
#define ENCODER

#include <common.hpp>

namespace whisper_library {

/*
	Encoder is the abstract class every encoder has to inherit from. You can assign an encoder to a CovertChannel.
*/
class WHISPERAPI Encoder {

public:
	// constructor
	Encoder();

};
}
#endif // ENCODER