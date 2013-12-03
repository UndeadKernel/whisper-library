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
	  
	  int set_sourcePort(); // 16bit
	  int set_destPort(); // 16 bit
	  
	  int set_sequenceNumber(); // 32 bit
	  int set_acknowlageNumber(); // 32 bit
	  
	  int set_dataOffset(); // 4 bit
	  int set_reserved(); // 3 bit
	  
	  int set_flags(); // 9 bit
	  bool set_ns();
	  bool set_cwr();
	  bool set_ece();
	  bool set_urg();
	  bool set_ack();
	  bool set_psh();
	  bool set_rst();
	  bool set_syn();
	  bool set_fin();

	  int set_windowSize();
	  int set_checksum();
	  int *set_options();
	  
  
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
	  int  options[10];
  };
}
