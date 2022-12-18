//***************************************************************************
//
//  Cesar Caballero
//  z1944013
//  CSCI 463-MSTR
//  Assignment 
//  Due date: 11/04/22
//
//  I certify that this is my own work and where appropriate an extension 
//  of the starter code provided for the assignment.
//
//***************************************************************************

#ifndef H_HEX
#define H_HEX

#include <string>

/**
 * @brief hex class with functions that print numbers as strings.
 * 
 */
class hex
{
public :
    static std::string to_hex8 ( uint8_t i );
    static std::string to_hex32 ( uint32_t i );
    static std::string to_hex0x32 ( uint32_t i );
    static std::string to_hex0x20(uint32_t i);
    static std::string to_hex0x12(uint32_t i);
};


#endif