#include "tcppackage.hpp"

namespace whisperLibrary {
	TcpPackage::TcpPackage(){
		
	}
	 
	int TcpPackage::get_sourcePort(){
		return sourcePort;
	} // 16bit
	int TcpPackage::get_destPort(){
		return destPort;
	} // 16 bit
	  
	int TcpPackage::get_sequenceNumber(){
		return sequenceNumber;
	}; // 32 bit
	int TcpPackage::get_acknowlageNumber(){
		return acknowlageNumber;
	}; // 32 bit
	  
	int TcpPackage::get_dataOffset(){
		return dataOffset;
	}; // 4 bit
	int TcpPackage::get_reserved(){
		return reserved;
	}; // 3 bit
	  
	int TcpPackage::get_flags(){
		return 0;
	}; // 9 bit
	bool TcpPackage::get_ns(){
		return ns;
	};
	bool TcpPackage::get_cwr(){
		return cwr;
	};
	bool TcpPackage::get_ece(){
		return ece;
	};
	bool TcpPackage::get_urg(){
		return urg;
	};
	bool TcpPackage::get_ack(){
		return ack;
	};
	bool TcpPackage::get_psh(){
		return psh;
	};
	bool TcpPackage::get_rst(){
		return rst;
	};
	bool TcpPackage::get_syn(){
		return syn;
	};
	bool TcpPackage::get_fin(){
		return fin;
	};

	int TcpPackage::get_windowSize(){
		return windowSize;
	};
	int TcpPackage::get_checksum(){
		return checksum;
	};
	int* TcpPackage::get_options(){
		return options;
	};
	  
	void TcpPackage::set_sourcePort(int val){
		sourcePort = val;
	}; // 16bit
	void TcpPackage::set_destPort(int val){
		destPort = val;
	}; // 16 bit
	  
	void TcpPackage::set_sequenceNumber(int val){
		sequenceNumber = val;
	}; // 32 bit
	void TcpPackage::set_acknowlageNumber(int val){
		acknowlageNumber = val;
	}; // 32 bit
	  
	void TcpPackage::set_dataOffset(int val){
		dataOffset = val;
	}; // 4 bit
	void TcpPackage::set_reserved(int val){
		reserved = val;
	}; // 3 bit
	  
	void TcpPackage::set_flags(int val){
		
	}; // 9 bit
	void TcpPackage::set_ns(bool val){
		ns = val;
	};
	void TcpPackage::set_cwr(bool val){
		cwr = val;
	};
	void TcpPackage::set_ece(bool val){
		ece = val;
	};
	void TcpPackage::set_urg(bool val){
		urg = val;
	};
	void TcpPackage::set_ack(bool val){
		ack = val;
	};
	void TcpPackage::set_psh(bool val){
		psh = val;
	};
	void TcpPackage::set_rst(bool val){
		rst = val;
	};
	void TcpPackage::set_syn(bool val){
		syn = val;
	};
	void TcpPackage::set_fin(bool val){
		fin = val;
	};

	void TcpPackage::set_windowSize(int val){
		windowSize = val;
	};
	void TcpPackage::set_checksum(int val){
		checksum = val;
	};
	void TcpPackage::set_options(int* val){
		options = val;
	};
}
