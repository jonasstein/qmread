#include "lmfile.hpp"
#include <inttypes.h>
#include <istream>
#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <string>     // std::string, std::stoull
#include <stdio.h>
//#include <stdint.h>
#include <byteswap.h> // http://stackoverflow.com/questions/105252/how-do-i-convert-between-big-endian-and-little-endian-values-in-c
#include <iomanip>
#include <cstdint>


#define _DEBUG
#ifdef _DEBUG
#define SHOW(a) std::cout << #a << ": 0x" << std::hex << (a) << std::endl
#else
#define SHOW(a)
#endif


lmfile::lmfile() : filesize ( 0 ), file_last_position_after_signature ( 0 ), events ( 0 )
{

}

lmfile::~lmfile()
{
}


uint64_t lmfile::getfilesize()
{
  return lmfile::filesize;
}


uint64_t lmfile::read64bit ( std::ifstream& ifs )
{
  uint64_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),8 );

// nb: little endian!
  return __builtin_bswap64 ( sequenceRAW );
}

uint64_t lmfile::read48bit ( std::ifstream& ifs )
{
  uint64_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),6 );
// nb: little endian!
  return __builtin_bswap64 ( sequenceRAW );
}


uint16_t lmfile::read16bit ( std::ifstream& ifs )
{
  uint16_t sequenceRAW;
  ifs.read ( reinterpret_cast<char *> ( &sequenceRAW ),2 );
  // nb: little endian!
  return __builtin_bswap16 ( sequenceRAW );
}


void lmfile::skipparameter ( std::ifstream& ifs )
{
  //for now it skips 4x 42 Bit parameter 0..3
  ifs.ignore ( 24 );
}

int lmfile::getsignaturetype ( std::ifstream& ifs )
{
  const uint64_t headersignature = 0x00005555AAAAFFFF;
  const uint64_t blocksignature = 0x0000FFFF5555AAAA;
  const uint64_t filesignature = 0xFFFFAAAA55550000;

  uint64_t sequence = read64bit ( ifs );

  file_last_position_after_signature = ifs.tellg();

  switch ( sequence )
    {
    case headersignature:
      lmfile::file_last_signature_type = 1;
      return 1;

    case blocksignature:
      lmfile::file_last_signature_type = 2;
      return 2;

    case filesignature:
      lmfile::file_last_signature_type = 3;
      return 3;

    default:
      std::cerr << ( "Error: signature not found" );
      lmfile::file_last_signature_type = -1;
      return -1;
    }
}


void lmfile::printstatus()
{
  
  printf ( "Listmode file Status\n====================\n" );
  std::cout << std::setw(21) << std::left << "Filesize (Byte)"  << std::dec << lmfile::filesize << std::endl;
  std::cout << std::setw(21) << std::left << "Lastsig type"    << std::dec << lmfile::file_last_signature_type << std::endl;  
  std::cout << std::setw(21) << std::left << "db.bufferlength"    << std::dec << db.bufferlength << std::endl;  
  std::cout << std::setw(21) << std::left << "db.header_timestamp"    << std::dec << db.header_timestamp << std::endl;  
  std::cout << std::setw(21) << std::left << "events"    << std::dec << events << std::endl;  
}


void lmfile::parseevent ( std::ifstream& ifs )
{
  char thisevent[6];
  ifs.getline ( thisevent,6 );
  events++;
}

// uint16_t events_in_buffer ( uint16_t bufferlength_in_words )
// {
//   const uint64_t bytes_per_header =  42;
//   const uint64_t bytes_per_event =  6;
//   uint64_t bufferlength_in_bytes = bufferlength_in_words * 2;
//   uint64_t bytes_used_by_events;
// 
//   bytes_used_by_events = ( bufferlength_in_bytes - bytes_per_header );
// 
//   uint16_t myreturn =0;
//   
//   if ( ( bytes_used_by_events % bytes_per_event ) == 0 )
//     {
//       myreturn = bytes_used_by_events / bytes_per_event; // = number of events; max (1500-42)/6 = 243 events
//     }
//   else
//     {
//       std::cerr << "Error: Wrong bufferlength. Should be 42 + 6n Byte.";
//     }
// 
//   return myreturn;
// }

void lmfile::parsedatablockheader ( std::ifstream& ifs )
{
  db.bufferlength = read16bit ( ifs ); // in 16 bit words
  //uint16_t number_of_events = events_in_buffer(bufferlength_in_words);
  
  db.buffertype = read16bit ( ifs );
  db.headerlength = read16bit ( ifs );
  db.buffernumber = read16bit ( ifs );
  std::cout << "buffernumber: " << std::dec << db.buffernumber << std::endl;
  
  db.runid = read16bit ( ifs );
  std::cout << "runid: " << std::dec << db.runid << std::endl;
  SHOW ( db.runid );

  uint16_t mcpdid_and_status = read16bit ( ifs );
  db.mcpdid = (mcpdid_and_status & 0xFF00);
  db.daqrunning = 1; // TODO pick the right bit later...
  db.syncok = 1; // TODO pick the right bit later...
  
  uint64_t new_header_timestamp = read48bit ( ifs );
  if ( db.header_timestamp==0 )
    {
      db.header_timestamp = new_header_timestamp; // first data buffer
    }
  else
    {
      db.header_timestamp = new_header_timestamp; // TODO make some tests here. new > last?
    }

  lmfile::skipparameter ( ifs );
}


void lmfile::readheader ( std::ifstream& ifs )
{
  // read first line and parse secondline with number of lines
  char thisline[80];
  
  ifs.getline ( thisline,80 ); // TODO test, if it is "mesytec psd listmode data"
  ifs.getline ( thisline,80 ); // header length: nnnnn lines
  
  uint32_t header_number_of_ascii_lines = 0;
  sscanf ( thisline,"header length: %d lines\n",&header_number_of_ascii_lines );

  // read next headerlines, if there are > 2

  char nextheaderlines[80];
  for (uint32_t i = header_number_of_ascii_lines - 2; i>0 ; i-- )
    {
      ifs.getline ( nextheaderlines,80 );
    }
  //TODO needs testing with a file with many header lines
  SHOW ( header_number_of_ascii_lines);

  // check for header signature
  int sigtype = getsignaturetype ( ifs );
  SHOW ( sigtype  );
 }

 
 
void lmfile::importfile ( const char* filename )
{
  // measure file size
  std::ifstream ifs ( filename, std::ifstream::ate | std::ifstream::binary );
  filesize = ifs.tellg();


  // set position to file start
  ifs.seekg ( 0 );

  lmfile::readheader(ifs);

  // while not EOF or NEXT8BYTES = signature
  //    read datablock header 42 Byte

  parsedatablockheader ( ifs );

  //    read n events
   uint16_t i=0;
   uint16_t number_of_events = (db.bufferlength - 21 ) / 3;
   for ( i = number_of_events; i>0 ; i-- )
     {
       std::cout << std::dec << i << std::endl;
       lmfile::parseevent ( ifs );
     }

  
  ifs.close();
}







