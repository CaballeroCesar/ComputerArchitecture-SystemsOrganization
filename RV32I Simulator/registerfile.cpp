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
#include <iostream>
#include <iomanip>
#include "registerfile.h"
#include "hex.h"

/**
 * @brief Constructor for registerfile class
 * 
 */
registerfile::registerfile()
{
    reset();
}

/**
 * @brief set register 0 to 0 and the rest to 0xf0f0f0f0
 * 
 */
void registerfile::reset()
{
    int i = 1;
    while(i < 32)
    {
        registers.push_back(0xf0f0f0f0);
        i++;
    }
    registers.at(0) = 0;
    
    
}

/**
 * @brief assign register a value
 * 
 * @param r = register
 * @param val value assigned to the register
 */
void registerfile::set(uint32_t r, int32_t val)
{
    if(r != 0)
    {
        registers.at(r) = val;
    }
}

/**
 * @brief return value of the register
 * 
 * @param r register to get value from
 * @return int32_t return the value as 32bit signed integer
 */
int32_t registerfile::get(uint32_t r) const
{
    if(r != 0)
    {
        return registers.at(r);
    }
    else return 0;
}

/**
 * @brief dump out contents of the registers
 * 
 * @param hdr string printed at the beginning of the line
 */
void registerfile::dump(const std::string &hdr) const
{
    for(uint32_t i=0; i<32; i++)
    {
        if(i % 8 == 0)
        {
            if(i != 0)
            {
                std::cout << std::endl;
            }
            std::cout << hdr;

            std::ostringstream os;
            os << "x" << i;
            std::cout << std::setw(3) << std::right << os.str();
        }
        if(i%8 != 0 && i%4 == 0)
        {
            std::cout << " ";
        }
        std::cout << " " << hex::to_hex32(registers.at(i));
    }
}
