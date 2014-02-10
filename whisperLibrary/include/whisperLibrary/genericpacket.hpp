// Author: Martin Oehler

#ifndef GENERIC_PACKET
#define GENERIC_PACKET

#include <vector>

using namespace std;
namespace whisper_library {

	class GenericPacket {
	public:
		GenericPacket() {};
		GenericPacket(vector<bool> content) : m_content(content) {};
		void setContent(vector<bool> content);
		vector<bool> content();
	private:
		vector<bool> m_content;
	};

}

#endif // GENERIC_PACKET