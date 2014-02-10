#include <genericpacket.hpp>

namespace whisper_library {
	vector<bool> GenericPacket::content() {
		return m_content;
	}

	void GenericPacket::setContent(vector<bool> content) {
		m_content = content;
	}
}