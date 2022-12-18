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

#ifndef H_CPU_SINGLE_HART
#define H_CPU_SINGLE_HART

#include "rv32i_hart.h"
#include "memory.h"
#include "registerfile.h"

/**
 * @brief subclass of rv32i_hart to represent CPU with a single hart
 * 
 */
class cpu_single_hart : public rv32i_hart
{
public:
    /**
     * @brief Construct a new cpu single hart object
     * 
     * @param mem memory that gets used from rv32i_hart class
     */
    cpu_single_hart(memory &mem) : rv32i_hart(mem) {}
    
    void run(uint64_t exec_limit);
};

#endif