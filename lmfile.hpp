#ifndef LISTMODELIB_H
#define LISTMODELIB_H

#include <cstdint> 
#include <inttypes.h>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <iomanip>

struct datablock{
     uint16_t bufferlength;
     uint16_t buffertype;
     uint16_t headerlength; // number of 16 bit words in the list mode file header
     uint16_t buffernumber;
     uint16_t runid;
     uint8_t mcpdid;
     bool daqrunning;
     bool syncok;
     uint64_t header_timestamp=0;
   };

class lmfile
{
  private:
    uint64_t filesize;
    uint64_t file_last_position_after_signature; // points to first char behind the last signature
    int file_last_signature_type; // 1=header, 2=databuffer, 3= eofsig, -1=else
    datablock db; 
    uint64_t eventtime[1000000]; // TODO save eventtime and eventtype as two arrays time[] type[], or one array per type ch0[] ch1[] ch2[] ch3[]? 
    uint64_t events; //number of events
    
  public:
    lmfile();
    ~lmfile();
    
  uint64_t getfilesize();
  void importfile(const char* filename);
  int getsignaturetype(std::ifstream& ifs);
  void printstatus();
  void parseevent(std::ifstream& ifs);
  void parsedatablockheader(std::ifstream& ifs);
  void skipparameter(std::ifstream& ifs);
  uint16_t events_in_buffer ( uint16_t bufferlength_in_words );
  uint64_t read64bit ( std::ifstream& ifs );
  uint64_t read48bit ( std::ifstream& ifs );
  uint16_t read16bit ( std::ifstream& ifs );
  void readheader ( std::ifstream& ifs );

  //void printstatus();
};

#endif // LISTMODELIB_H
