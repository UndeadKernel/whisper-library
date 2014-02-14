#ifndef UDP_HEADER
#define UDP_HEADER

#include <vector>

using namespace std;

namespace whisper_library {
	class UdpPacket {
	public:
		vector<bool> packet();
	};
}

#endif