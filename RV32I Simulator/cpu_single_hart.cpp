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
#include "registerfile.h"
#include "hex.h"
#include "rv32i_decode.h"
#include "memory.h"
#include "rv32i_hart.h"
#include "cpu_single_hart.h"

/**
 * @brief run simulated memory
 * 
 * @param exec_limit parameter to know when to stop loop
 */
void cpu_single_hart::run(uint64_t exec_limit)
{
    regs.set(2, mem.get_size());

    while(is_halted() == false && (exec_limit == 0 || exec_limit > get_insn_counter()))
    {
        tick();
    }
    if(is_halted() == true)
    {
        std::cout << "Execution terminated. Reason:" << get_halt_reason() << std::endl;
    }
    std::cout << get_insn_counter() << " instructions executed" << std::endl; 
}