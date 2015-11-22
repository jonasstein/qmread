#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
#include <stdint.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c

#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a) 
#endif

class lmfile{
  private:
    uint64_t filesize;
    uint64_t headerlength; // number of 16 bit words in the list mode file header
    uint64_t file_last_position_after_signature; // points to first char behind the last signature
    
    
  public:
    lmfile();
    ~lmfile();
    
  uint64_t getfilesize();
  void importfile(const char* filename);
  void printstatus();

};

lmfile::lmfile() : filesize(0), headerlength(0), file_last_position_after_signature(0)
{
  
} 

lmfile::~lmfile()
{
 
}



uint64_t getfilesize() {
    return filesize;
}
  

uint64_t read64bit(std::ifstream& ifs)
{
  uint64_t sequenceRAW; 
  ifs.read(reinterpret_cast<char *>(&sequenceRAW),8);

// nb: little endian!  
return __builtin_bswap64(sequenceRAW);
}

uint64_t read48bit(std::ifstream& ifs)
{
  uint64_t sequenceRAW; 
  ifs.read(reinterpret_cast<char *>(&sequenceRAW),6);
// nb: little endian!  
return __builtin_bswap64(sequenceRAW);
}


uint16_t read16bit(std::ifstream& ifs)
{
  uint16_t sequenceRAW; 
  ifs.read(reinterpret_cast<char *>(&sequenceRAW),2);

// nb: little endian!  
  return __builtin_bswap16(sequenceRAW);
}



int getsignaturetype(std::ifstream& ifs)
{
  const uint64_t headersignature = 0x00005555AAAAFFFF;
  const uint64_t blocksignature = 0x0000FFFF5555AAAA;
  const uint64_t filesignature = 0xFFFFAAAA55550000;
  
  uint64_t sequence = read64bit(ifs);  
  
  file_last_position_after_signature = ifs.tellg();
   
  switch(sequence)
  {
    case headersignature:
      return 1;
      
    case blocksignature:
      return 2;
      
    case filesignature:
      return 3;
      
    default:
      std::cerr << ("Error: signature not found");
      return -1;
  }
}


void printstatus()
{
  
}


void parseevent(std::ifstream& ifs)
{
  char thisevent[6];
  ifs.getline(thisevent,6);
}

void parsedatablockheader(std::ifstream& ifs)
{
  uint16_t bufferlength = read16bit(ifs); 
  uint16_t buffertype = read16bit(ifs); 
  uint16_t headerlength = read16bit(ifs); 
  uint16_t buffernumber = read16bit(ifs); 
  std::cout << "buffernumber: " << std::dec << buffernumber << std::endl;
  uint16_t runid = read16bit(ifs); 
  std::cout << "runid: " << std::dec << runid << std::endl;
  SHOW(runid);
  
  // TODO CONTINUE HERE
  // ghex shows 159 events in the first block. 
  // how do these values match? What is bufferlength and headerlength???
  
  int i=0;
  for (i = bufferlength; i>0 ; i--)
  {
    std::cout << std::dec << i << std::endl;
    parseevent(ifs);
  }
}    
    

void importfile(const char* filename)
  {
    // measure file size
    std::ifstream ifs(filename, std::ifstream::ate | std::ifstream::binary);
    filesize = ifs.tellg();
        
    
    // set position to file start
    ifs.seekg(0);

    // read first line and parse secondline with number of lines
    char firstline[80];
    char secondline[80];
     
    ifs.getline(firstline,80);
    ifs.getline(secondline,80);
    sscanf (secondline,"header length: %d lines\n",&headerlength);

    // read next headerlines, if there are > 2
    
    char nextheaderlines[80];
    int i = 0;
    for (i = headerlength - 2; i>0 ; i--){ifs.getline(nextheaderlines,80);}
      //TODO needs testing with a file with many header lines
    SHOW(headerlength);    

    // check for header signature
    SHOW(getsignaturetype(ifs));
    

    // while not EOF or NEXT8BYTES = signature 
    //    read datablock header 42 Byte
    
    parsedatablockheader(ifs);
    
    //    read n events
    
  ifs.close();
  }
  
  



//   struct datablock{
//     uint16_t bufferlength;
//     uint16_t buffertype;
//     uint16_t headerlength;
//     uint16_t buffernumber;
//     uint16_t runid;
//     uint8_t mcpdid;
//     bool daqrunning;
//     bool syncok;
//   } db;

  