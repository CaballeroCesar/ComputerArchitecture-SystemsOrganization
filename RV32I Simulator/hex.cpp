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
#include <iostream>
#include <iomanip>
#include <sstream>

/**
 * @brief Function that prints out a hex digit as its 8 bit representation
 * 
 * @param i = input number to be converted 
 * @return std::string hex digit is stored as string and outputted
 */
std::string hex::to_hex8(uint8_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(2) << static_cast<uint16_t>(i);
    return os.str();
}

/**
 * @brief Function that prints out a hex digit as its 32 bit representation
 * 
 * @param i = to be conveted to 32 bit
 * @return std::string hex digit is stored as string and outputted
 */
std::string hex::to_hex32(uint32_t i)
{
    std::ostringstream os;
    os << std::hex << std::setfill('0') << std::setw(8) << i;
    return os.str();
}

/**
 * @brief Uses the to_hex32() function, but concatenates "0x" before 32 bit representation
 * 
 * @param i = is converted to 32 bit
 * @return std::string hex digit stored as string and outputted
 */
std::string hex::to_hex0x32(uint32_t i)
{
    return std::string("0x")+to_hex32(i);
}

/**
 * @brief return a hex digit starting with "0x" and 5 hex digits after that
 * 
 * @param i = input that is converted into the hex digit
 * @return std::string hex digit as as a string value
 */
std::string hex::to_hex0x20(uint32_t i)
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setfill('0') << std::setw(5) << i;
    return os.str();
}

/**
 * @brief return a hex digit starting with "0x" and then 3 hex digits after representing 12 least sig bits 
 * 
 * @param i = input that will be converted to hex digit
 * @return std::string hex digit as a string value
 */
std::string hex::to_hex0x12(uint32_t i)
{
    std::ostringstream os;
    os << "0x" << std::hex << std::setfill('0') << std::setw(3) << (i & 0xfff);
    return os.str();
}
