//***************************************************************************
//
//  Cesar Caballero
//  z1944013
//  CSCI 463-MSTR
//  Assignment 5
//  Due date: 11/04/22
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************


#include "hex.h"
#include "memory.h"
#include <iostream>
#include <iomanip>
#include <fstream>

/**
 * @brief distribute siz bytes from the mem vector. Have each byte initialized to 0xa5
 * 
 * @param siz = size of bytes allocated to the vector
 */
memory::memory(uint32_t siz)
{
   siz = (siz+15)&0xfffffff0; // round the length up, mod-16
   mem.resize(siz,0xa5); 
}

/**
 * @brief Destroy the memory::memory object
 * 
 */
memory::~memory()
{

}

/**
 * @brief check to see if an address has an actual byte
 * 
 * @param i = address to be checked
 * @return true if not a legal address
 * @return false when address is legal
 */
bool memory::check_illegal(uint32_t i) const
{
   if( i >= memory::get_size())
   {
      std::cout << "WARNING: Address out of range: " << hex::to_hex0x32(i) << std::endl;
      return true;
   }
   else return false;
}

/**
 * @brief function that returns size of bytes stored in vector
 * 
 * @return uint32_t = unsigned integer holding size stored
 */
uint32_t memory::get_size() const
{
   return mem.size();
}

/**
 * @brief Ensure address is legal.Then return one byte from address
 * 
 * @param addr = index to search for byte
 * @return uint8_t = ensuring byte is returned as unsigned 8 bits 
 */
uint8_t memory::get8(uint32_t addr) const
{
   if(!memory::check_illegal(addr))
   {
      return mem[addr];
   }
   else return 0;
}

/**
 * @brief  calls get8() twice stored into separate variables. amd then combined into little-endian formatting
 * 
 * @param addr = index of where to retrieve the 2 bytes
 * @return uint16_t 2 bytes as little endian formatting using bitwise operators
 */
uint16_t memory::get16(uint32_t addr) const
{
   uint8_t secondByte = get8(addr);
   uint8_t firstByte = get8(addr+1);
   uint16_t littleEndian = ((uint16_t)firstByte << 8) + secondByte;
   return (uint16_t)littleEndian;
}

/**
 * @brief calls get16() twice stored into separate variables. amd then combined into little-endian formatting
 * 
 * @param addr = index on where the bytes are being retrieved from
 * @return uint32_t = formatting to properly combine 16 bits into 32 bits.
 */
uint32_t memory::get32(uint32_t addr) const
{
   uint16_t secondByte = get16(addr);
   uint16_t firstByte = get16(addr + 2);
   uint32_t littleEndian = ((uint32_t)firstByte << 16) + secondByte;
   return (uint32_t)littleEndian;
}

/**
 * @brief sign extend 8 bits into 32 bits
 * 
 * @param addr = index of in memory
 * @return int32_t sign extended 8 bit value
 */
int32_t memory::get8_sx(uint32_t addr) const
{
   int32_t value = get8(addr);
   return (int32_t)(value | (value&0x00000080 ? 0xffffff00 : 0));
}

/**
 * @brief sign extend 16 bits as 32 bits
 * 
 * @param addr = index in memory
 * @return int32_t to be able to sign extend 16 bit value
 */
int32_t memory::get16_sx(uint32_t addr) const
{
   int32_t value = get16(addr);
   return (int32_t)(value | (value&0x00000080 ? 0xffff0000 : 0));
}

/**
 * @brief  get 32bits and turn it into a signed integer value
 * 
 * @param addr = value where to look in memory
 * @return int32_t to cast value as a signed integer
 */
int32_t memory::get32_sx(uint32_t addr) const
{
   return (int32_t)(memory::get32(addr));
}

/**
 * @brief verifies address is legal. if so set byte to given value
 * 
 * @param addr index in memory
 * @param val stores the value into the vector
 */
void memory::set8(uint32_t addr, uint8_t val)
{
   if(!memory::check_illegal(addr)) // if address is not illegal
   {
      mem[addr] = val;
   }
}

/**
 * @brief take 2 bytes and separate them into 1 byte each and stored in little endian
 * 
 * @param addr  =index to check in memory
 * @param val = value to separate
 */
void memory::set16(uint32_t addr, uint16_t val)
{
   uint8_t partA = (uint8_t)((val & 0xFF00) >> 8);
   uint8_t partB = (uint8_t)(val & 0x00FF);
   memory::set8(addr + 1, partA);
   memory::set8(addr, partB);
}

/**
 * @brief get 32 bytes and separate them into 16 bytes to store in memory in little endian
 * 
 * @param addr = index where bytes retrieved
 * @param val = value that is separated
 */
void memory::set32(uint32_t addr, uint32_t val)
{
   uint16_t partA = (uint16_t)((val & 0xFFFF0000) >> 16);
   uint16_t partB = (uint16_t)(val & 0x0000FFFF);
   memory::set16(addr + 2, partA);
   memory::set16(addr, partB);
}

/**
 * @brief print out contents of mem vector to mimic a hexdump
 * 
 */
void memory::dump() const
{
   std::string space = " ";
   std::string character; // string to load ch into and print at once 
   
   for (uint32_t i = 0; i < get_size() ; ++i) 
   {
      if (i % 16 == 0) // print : once 16 bits are reached
      {
         std::cout << to_hex32(i) << ": ";
      } 

      std::cout << to_hex8(get8(i)) << space; // extra space

      uint8_t ch = get8(i);
      ch = isprint(ch) ? ch : '.';
      character += ch; // load ch into character string

      if (i %16 == 7)
      {
         std::cout << space; //print space after 8 bits
      }

      if (i % 16 == 15) 
      {
         std::cout << "*" << character << "*" << std::endl; // print out character string after 16 more bits
         character.clear(); // clear string to restart loop
      }
   }
}

/**
 * @brief open a file in binary mode to be able to store contents into mem vector
 * 
 * @param fname file that will be opened
 * @return true when file is properly read in
 * @return false when there is a problem opening file properly 
 */
bool memory::load_file(const std::string &fname)
{
   std::ifstream infile(fname, std::ios::in|std::ios::binary);
   if(!infile.is_open())
   {
      std::cerr << "Can't open file '" << fname << "' for reading." << std::endl;
      return false;
   }
   
   uint8_t i;
   infile >> std::noskipws;
   for (uint32_t addr = 0; infile >> i; ++addr)
   {
      if(check_illegal(addr))
      {
         std::cerr << "Program too big." << std::endl;
         infile.close();
         return false;
      }
      else 
      set8(addr,i);
   }
      
   infile.close();
   return true;
}
