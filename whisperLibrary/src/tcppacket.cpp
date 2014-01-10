#include "tcppacket.hpp"

namespace whisperLibrary {
    TcpPacket::TcpPacket(){
        _header.resize(159);
        _options.resize(319);
	}
	
	/* Locations of the header informations:
		 * 0-15 Source port, 16-31 Destination port
		 * 32-63 Sequence number
		 * 64-95 Acknowledgement number
		 * 96-99 Data offset
		 * 100-102 Reserved
		 * 103 NS, 104 CWR, 105 ECE, 106 URG, 107 ACK
		 * 108 PSH, 109 RST, 110 SYN, 111 FIN, 112-127 Window Size
		 * 128-143 checksum, 144-159 Urgent Pointer
		 * Options field:
		 * 0-319 depending on data-offset
		 * 0-7 option kind, 8-15 option length
		 * (option-length) option data
		 */
	 
	// header: bits 0-15
    uint TcpPacket::sourcePort(){
        return vectorToUInt(0, 15, _header);
	} 
	// header: bits 16-31
    uint TcpPacket::destPort(){
        return vectorToUInt(16, 31, _header);
	} 
	// header: bits 32-63  
    uint TcpPacket::sequenceNumber(){
        return vectorToUInt(32, 63, _header);
    }
	// header: bits 64-95 
    uint TcpPacket::acknowlageNumber(){
        return vectorToUInt(64, 95, _header);
    }
	// header: bits 96-99  
    uint TcpPacket::dataOffset(){
        return vectorToUInt(96, 99, _header);
    }
	// header: bits 100-102
    uint TcpPacket::reserved(){
        return vectorToUInt(100, 102, _header);
    }
	// header: bits 103-111  
    std::vector<bool>* TcpPacket::flags(){
        std::vector<bool>* ret = new std::vector<bool>();
        for (int i = 103; i <= 111; i++){
            ret->push_back(_header.at(i));
        }
        return ret;
    }
	// header: bit 103
    bool TcpPacket::ns(){
        return _header.at(103);
    }
	// header: bit 104
    bool TcpPacket::cwr(){
        return _header.at(104);
    }
	// header: bit 105
    bool TcpPacket::ece(){
        return _header.at(105);
    }
	// header: bit 106
    bool TcpPacket::urg(){
        return _header.at(106);
    }
	// header: bit 107
    bool TcpPacket::ack(){
        return _header.at(107);
    }
	// header: bit 108
    bool TcpPacket::psh(){
        return _header.at(108);
    }
	// header: bit 109
    bool TcpPacket::rst(){
        return _header.at(109);
    }
	// header: bit 110
    bool TcpPacket::syn(){
        return _header.at(110);
    }
	// header: bit 111
    bool TcpPacket::fin(){
        return _header.at(111);
    }
	// header: bits 112-127
    uint TcpPacket::windowSize(){
        return vectorToUInt(112, 127, _header);
    }
	// header: bits 128-143
    uint TcpPacket::checksum(){
        return vectorToUInt(128, 143, _header);
    }
	// options: bits 0-320
    std::vector<bool>* TcpPacket::options(){
        return new std::vector<bool>(_options);
    }
    // packet
    std::vector<bool>* TcpPacket::packet(){
        std::vector<bool>* ret = new std::vector<bool>(_header);
        for (int i = 0; i < _options.size(); i++){
            ret->push_back(_options[i]);
        }
        for (int i = 0; i < _data.size(); i++){
            ret->push_back(_data[i]);
        }
        return ret;
    }

    // data
    std::vector<bool>* TcpPacket::data(){
        return new std::vector<bool>(_data);
    }

	// header: bits 0-15  
    void TcpPacket::set_sourcePort(uint val){
        uIntToVector(0,15,_header,val);
    }
	// header: bits 16-31
    void TcpPacket::set_destPort(uint val){
        uIntToVector(16,31,_header,val);
    }
	// header: bits 32-63  
    void TcpPacket::set_sequenceNumber(uint val){
        uIntToVector(32,63,_header,val);
    }
	// header: bits 64-95
    void TcpPacket::set_acknowlageNumber(uint val){
        uIntToVector(64,95,_header,val);
    }
	// header: bits 96-99  
    void TcpPacket::set_dataOffset(uint val){
        uIntToVector(96,99,_header,val);
    }
	// header: bits 100-102
    void TcpPacket::set_reserved(uint val){
        uIntToVector(100,102,_header,val);
    }
	// header: bits 103-111  
    void TcpPacket::set_flags(std::vector<bool> &val){
        for (int i = 103; i <= 111; i++){
            _header[i] = val.at(i - 103);
        }
    }
	// header: bit 103
    void TcpPacket::set_ns(bool val){
        _header[103] = val;
    }
	// header: bit 104
    void TcpPacket::set_cwr(bool val){
        _header[104] = val;
    }
	// header: bit 105
    void TcpPacket::set_ece(bool val){
        _header[105] = val;
    }
	// header: bit 106
    void TcpPacket::set_urg(bool val){
        _header[106] = val;
    }
	// header: bit 107
    void TcpPacket::set_ack(bool val){
        _header[107] = val;
    }
	// header: bit 108
    void TcpPacket::set_psh(bool val){
        _header[108] = val;
    }
	// header: bit 109
    void TcpPacket::set_rst(bool val){
        _header[109] = val;
    }
	// header: bit 110
    void TcpPacket::set_syn(bool val){
        _header[110] = val;
    }
	// header: bit 111
    void TcpPacket::set_fin(bool val){
        _header[111] = val;
    }
	// header: bits 112-127
    void TcpPacket::set_windowSize(uint val){
        uIntToVector(112,127,_header,val);
    }
	// header: bits 128-143
    void TcpPacket::set_checksum(uint val){
        uIntToVector(128,143,_header,val);
    }
	// options: bits 0-15
    void TcpPacket::set_options(std::vector<bool> &val){
        _options = val;
    }
	
    bool* TcpPacket::intToBoolArray(uint val){
        bool* ret = new bool[32];
        for(int i = 0; i<32; i++){
            if (val % 2 == 1)
                ret[i] = 1;
            val >> 1;
        }
        return ret;
    }
    uint TcpPacket::vectorToUInt(int start, int end, std::vector<bool> &set){
		uint ret = 0;
		for (int i = start; i <= end; i++){
			if 	(set.at(i))
				ret += 1 << (i - start);
		}
		return ret;
    }
    void TcpPacket::uIntToVector(int start, int end, std::vector<bool> &set, uint val){
		bool* ins = intToBoolArray(val);
		for (int i = start; i <= end; i++){
			set[i] = ins[i];
		}
    }
}
