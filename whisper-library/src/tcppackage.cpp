#include "tcppackage.hpp"

namespace whisperLibrary {
	TcpPackage::TcpPackage(){
		header.resize(159);
		options.resize(319);
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
	uint TcpPackage::get_sourcePort(){
		return vectorToUInt(0, 15, header);
	} 
	// header: bits 16-31
	uint TcpPackage::get_destPort(){
		return vectorToUInt(16, 31, header);
	} 
	// header: bits 32-63  
	uint TcpPackage::get_sequenceNumber(){
		return vectorToUInt(32, 63, header);
	};
	// header: bits 64-95 
	uint TcpPackage::get_acknowlageNumber(){
		return vectorToUInt(64, 95, header);
	};
	// header: bits 96-99  
	uint TcpPackage::get_dataOffset(){
		return vectorToUInt(96, 99, header);
	};
	// header: bits 100-102
	uint TcpPackage::get_reserved(){
		return vectorToUInt(100, 102, header);
	}; 
	// header: bits 103-111  
	uint TcpPackage::get_flags(){
		return vectorToUInt(103, 111, header);
	};
	// header: bit 103
	bool TcpPackage::get_ns(){
		return header.at(103);
	};
	// header: bit 104
	bool TcpPackage::get_cwr(){
		return header.at(104);
	};
	// header: bit 105
	bool TcpPackage::get_ece(){
		return header.at(105);
	};
	// header: bit 106
	bool TcpPackage::get_urg(){
		return header.at(106);
	};
	// header: bit 107
	bool TcpPackage::get_ack(){
		return header.at(107);
	};
	// header: bit 108
	bool TcpPackage::get_psh(){
		return header.at(108);
	};
	// header: bit 109
	bool TcpPackage::get_rst(){
		return header.at(109);
	};
	// header: bit 110
	bool TcpPackage::get_syn(){
		return header.at(110);
	};
	// header: bit 111
	bool TcpPackage::get_fin(){
		return header.at(111);
	};
	// header: bits 112-127
	uint TcpPackage::get_windowSize(){
		return vectorToUInt(112, 127, header);
	};
	// header: bits 128-143
	uint TcpPackage::get_checksum(){
		return vectorToUInt(128, 143, header);
	};
	// options: bits 0-320
	std::vector<uint> TcpPackage::get_options(){
		std::vector<uint> ret;
		for (int i = 0; i < (options.size() / 32); i++){
			ret.push_back(vectorToUInt(i, (i + 31), options));
		}
		return ret;
	};
	// header: bits 0-15  
	void TcpPackage::set_sourcePort(uint val){
		uIntToVector(0,15,header,val);
	};
	// header: bits 16-31
	void TcpPackage::set_destPort(uint val){
		uIntToVector(16,31,header,val);
	};
	// header: bits 32-63  
	void TcpPackage::set_sequenceNumber(uint val){
		uIntToVector(32,63,header,val);
	};
	// header: bits 64-95
	void TcpPackage::set_acknowlageNumber(uint val){
		uIntToVector(64,95,header,val);
	};
	// header: bits 96-99  
	void TcpPackage::set_dataOffset(uint val){
		uIntToVector(96,99,header,val);
	};
	// header: bits 100-102
	void TcpPackage::set_reserved(uint val){
		uIntToVector(100,102,header,val);
	};
	// header: bits 103-111  
	void TcpPackage::set_flags(uint val){
		uIntToVector(103,111,header,val);
	};
	// header: bit 103
	void TcpPackage::set_ns(bool val){
		header[103] = val;
	};
	// header: bit 104
	void TcpPackage::set_cwr(bool val){
		header[104] = val;
	};
	// header: bit 105
	void TcpPackage::set_ece(bool val){
		header[105] = val;
	};
	// header: bit 106
	void TcpPackage::set_urg(bool val){
		header[106] = val;
	};
	// header: bit 107
	void TcpPackage::set_ack(bool val){
		header[107] = val;
	};
	// header: bit 108
	void TcpPackage::set_psh(bool val){
		header[108] = val;
	};
	// header: bit 109
	void TcpPackage::set_rst(bool val){
		header[109] = val;
	};
	// header: bit 110
	void TcpPackage::set_syn(bool val){
		header[110] = val;
	};
	// header: bit 111
	void TcpPackage::set_fin(bool val){
		header[111] = val;
	};
	// header: bits 112-127
	void TcpPackage::set_windowSize(uint val){
		uIntToVector(112,127,header,val);
	};
	// header: bits 128-143
	void TcpPackage::set_checksum(uint val){
		uIntToVector(128,143,header,val);
	};
	// options: bits 0-15
	void TcpPackage::set_options(uint* val, uint size){
		for (int i = 0; i < size; i++){
			uIntToVector(i, (i + 31), options, val[i]);
		}
	};
	
	bool* TcpPackage::intToBoolArray(uint val){
		bool* ret = new bool[32];
		for(int i = 0; i<32; i++){
			if (val % 2 == 1)
				ret[i] = 1;
			val >> 1;
		}
		return ret;
	};
	uint TcpPackage::vectorToUInt(int start, int end, std::vector<bool> &set){
		uint ret = 0;
		for (int i = start; i <= end; i++){
			if 	(set.at(i))
				ret += 1 << (i - start);
		}
		return ret;
	};
	void TcpPackage::uIntToVector(int start, int end, std::vector<bool> &set, uint val){
		bool* ins = intToBoolArray(val);
		for (int i = start; i <= end; i++){
			set[i] = ins[i];
		}
	}; 
}
