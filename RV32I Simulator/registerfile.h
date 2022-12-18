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

#ifndef H_REGISTERFILE
#define H_REGISTERFILE

#include <string>
#include <vector>
/**
* @brief class to store the state of general purpose registers 
* 
*/
class registerfile 
{
public:
    registerfile();
    void reset();
    void set(uint32_t r, int32_t val);
    int32_t get(uint32_t r) const;
    void dump(const std::string &hdr) const;

private:
    std::vector<int32_t> registers;
};

#endif
