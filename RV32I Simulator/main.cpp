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
#include <sstream>
#include <unistd.h>
#include "hex.h"
#include "memory.h"
#include "rv32i_decode.h"
#include "rv32i_hart.h"
#include "cpu_single_hart.h"

using std::cout;
using std::cerr;
using std::endl;

static void usage ();
static void disassemble(const memory &mem);

/**
 * @brief function to print an error message when the command line usage is entered wrong, and how to properly use
 * 
 */
static void usage()
{
	cerr << "Usage: rv32i [-d] [-i] [-r] [-z] [-l exec-limit] [-m hex-mem-size] infile" << endl;
	cerr << "    -d show disassembly before program execution" << endl;
	cerr << "    -i show instruction printing during execution" << endl;
	cerr << "    -l maximum number of instructions to exec" << endl;
	cerr << "    -m specify memory size (default = 0x100)" << endl;
	cerr << "    -r show register printing during execution" << endl;
	cerr << "    -z show a dump of the regs & memory after simulation" << endl;
	exit(1);
}

/**
 * @brief dissasembles and print instruction using decode helper function
 * 
 * @param mem vector storing the memory that will be outputted
 */
static void disassemble(const memory &mem)
{
	uint32_t addr = 0;
	std::string decoded_insn;

	for(uint32_t i=0; i != mem.get_size(); i+=4) //loops after 4 bytes to new insn
	{
		decoded_insn = rv32i_decode::decode(addr,mem.get32(addr)); //store decoded insn from addr

		//printing: 32bit address in hex, 32bit hex fullword of instruction, decoded instruction
		std::cout << hex::to_hex32(addr) << ": " << hex::to_hex32(mem.get32(addr)) << "  " << decoded_insn << std::endl;

		addr += 4; // go to next address
	}
}

/**
 * @brief take in a binary file and dissasemble its contents with helper functions
 * 
 * @param argc argument counter
 * @param argv argument vector
 * @return int 0 when program runs successfully
 */
int main(int argc, char **argv)
{
	bool d = false;
	bool i = false;
	bool r = false;
	bool z = false;
	uint32_t memory_limit = 0x100; // default memory size = 256 bytes
    uint32_t execution_limit = 0;
	int opt;
	while ((opt = getopt(argc, argv, "m:l:dirz")) != -1)
	{
		switch (opt)
		{
		case 'm':
			{
			std::istringstream iss(optarg);
			iss >> std::hex >> memory_limit;
			}
			break;
		case 'd':
			{
				d = true;
			}
			break;
		case 'i':
			{
				i = true;
			}	
			break;
        case 'r':
			{
				r = true;				
			}
			break;
		case 'z':
			{
				z = true;				
			}
			break;
		case 'l':
			{
            std::istringstream iss(optarg);
			iss >> execution_limit;   
			}
			break;	
		default: /* ’?’ */
			usage();
		}
	}
	if (optind >= argc)
		usage(); // missing filename

	memory mem(memory_limit);

	if (!mem.load_file(argv[optind]))
		usage();

	cpu_single_hart cpu(mem);
	cpu.reset();

	if(d == true)
	{
		disassemble(mem);
	}
	if(i == true)
    {
		cpu.set_show_instructions(true);
	}
	if(r == true)
	{
		cpu.set_show_registers(true);
	}
    cpu.run(execution_limit);
	if(z == true)
	{
		cpu.dump();
		mem.dump();
	}
	
	return 0;
}

