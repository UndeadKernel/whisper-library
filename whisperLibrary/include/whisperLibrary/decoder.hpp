#ifndef DECODER
#define DECODER

#include <common.hpp>

namespace whisper_library {

/*
	Decoder is the abstract class every decoder has to inherit from. You can assign a Decoder to a CovertChannel.
*/
class WHISPERAPI Decoder {

public:
	// constructor
	Decoder();

};
}
#endif // DECODER