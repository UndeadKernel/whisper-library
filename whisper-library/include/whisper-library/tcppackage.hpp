//author: Jan Simon Bunten

#ifndef TCP_PACKAGE
#define TCP_PACKAGE
#endif

#include "common.hpp"

namespace whisperLibrary {

  class TcpPackage { 
  
  public:
	  TcpPackage();
	 
	  int get_sourcePort(); // 16bit
	  int get_destPort(); // 16 bit
	  
	  int get_sequenceNumber(); // 32 bit
	  int get_acknowlageNumber(); // 32 bit
	  
	  int get_dataOffset(); // 4 bit
	  int get_reserved(); // 3 bit
	  
	  int get_flags(); // 9 bit
	  bool get_ns();
	  bool get_cwr();
	  bool get_ece();
	  bool get_urg();
	  bool get_ack();
	  bool get_psh();
	  bool get_rst();
	  bool get_syn();
	  bool get_fin();

	  int get_windowSize();
	  int get_checksum();
	  int* get_options();
	  
	  void set_sourcePort(int val); // 16bit
	  void set_destPort(int val); // 16 bit
	  
	  void set_sequenceNumber(int val); // 32 bit
	  void set_acknowlageNumber(int val); // 32 bit
	  
	  void set_dataOffset(int val); // 4 bit
	  void set_reserved(int val); // 3 bit
	  
	  void set_flags(int val); // 9 bit
	  void set_ns(bool val);
	  void set_cwr(bool val);
	  void set_ece(bool val);
	  void set_urg(bool val);
	  void set_ack(bool val);
	  void set_psh(bool val);
	  void set_rst(bool val);
	  void set_syn(bool val);
	  void set_fin(bool val);

	  void set_windowSize(int val);
	  void set_checksum(int val);
	  void set_options(int* val);
	  
  
  private:
	  int sourcePort; // 16bit
	  int destPort; // 16 bit
	  
	  int sequenceNumber; // 32 bit
	  int acknowlageNumber; // 32 bit
	  
	  int dataOffset; // 4 bit
	  int reserved; // 3 bit
	  
	  bool ns;
	  bool cwr;
	  bool ece;
	  bool urg;
	  bool ack;
	  bool psh;
	  bool rst;
	  bool syn;
	  bool fin;

	  int windowSize;
	  int checksum;
	  int* options;
  };
}
