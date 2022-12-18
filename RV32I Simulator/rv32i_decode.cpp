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
#include <sstream>
#include <cassert>
#include "rv32i_decode.h"
#include "hex.h"
#include "memory.h"

/**
 * @brief decode an isntruction depending on what type it is
 * 
 * @param addr address of where instruction is
 * @param insn instruction to decode
 * @return std::string decoded instruction
 */
std::string rv32i_decode::decode(uint32_t addr, uint32_t insn)
{
switch(insn)
{  
case insn_ecall: return render_ecall(insn);     
case insn_ebreak: return render_ebreak(insn);   
} 

switch(get_opcode(insn))
{
default: return render_illegal_insn(insn);
case opcode_lui: return render_lui(insn);
case opcode_auipc: return render_auipc(insn);
case opcode_jal: return render_jal(addr,insn);
case opcode_jalr: return render_jalr(insn);
case opcode_btype:
    switch(get_funct3(insn))
    {
    default: return render_illegal_insn(insn);
    case funct3_beq: return render_btype(addr, insn, "beq");
    case funct3_bne: return render_btype(addr, insn, "bne");
    case funct3_blt: return render_btype(addr, insn, "blt");
    case funct3_bge: return render_btype(addr, insn, "bge");
    case funct3_bltu: return render_btype(addr, insn, "bltu");
    case funct3_bgeu: return render_btype(addr, insn, "bgeu");
    }
    assert(0 && "unrecognized funct3"); // impossible
case opcode_alu_imm:
    switch(get_funct3(insn))
    {
    default: return render_illegal_insn(insn);
    case funct3_add: return render_itype_alu(insn, "addi", get_imm_i(insn));
    case funct3_sll: return render_itype_alu(insn, "slli", get_imm_i(insn));
    case funct3_slt: return render_itype_alu(insn, "slti", get_imm_i(insn));
    case funct3_sltu: return render_itype_alu(insn, "sltiu", get_imm_i(insn));
    case funct3_xor: return render_itype_alu(insn, "xori", get_imm_i(insn));
    case funct3_or: return render_itype_alu(insn, "ori", get_imm_i(insn));
    case funct3_and: return render_itype_alu(insn, "andi", get_imm_i(insn));
    case funct3_srx:
        switch(get_funct7(insn))
        {
        default: return render_illegal_insn(insn);
        case funct7_sra: return render_itype_alu(insn, "srai", get_imm_i(insn)%XLEN);
        case funct7_srl: return render_itype_alu(insn, "srli", get_imm_i(insn)%XLEN);
        }
        assert(0 && "unrecognized funct7"); // impossible 
    }
    assert(0 && "unrecognized funct3"); // impossible
case opcode_load_imm:
    switch (get_funct3(insn))
    {
    default: return render_illegal_insn(insn);
    case funct3_lb:	return render_itype_load(insn, "lb");
    case funct3_lh:	return render_itype_load(insn, "lh");
    case funct3_lw: return render_itype_load(insn, "lw");
    case funct3_lbu: return render_itype_load(insn, "lbu");
    case funct3_lhu: return render_itype_load(insn, "lhu");
    }
case opcode_stype:
    switch (get_funct3(insn))
    {
    default: return render_illegal_insn(insn);
    case funct3_sb:	return render_stype(insn, "sb");
    case funct3_sh:	return render_stype(insn, "sh");
    case funct3_sw: return render_stype(insn, "sw");
    }
case opcode_rtype:
    switch(get_funct3(insn))
    {
    default: return render_illegal_insn(insn);
    case funct3_add:
        switch(get_funct7(insn))
        {
        default: return render_illegal_insn(insn);
        case funct7_add: return render_rtype(insn, "add");
        case funct7_sub: return render_rtype(insn, "sub");
        }
    case funct3_sll: return render_rtype(insn, "sll");
	case funct3_slt: return render_rtype(insn, "slt");
	case funct3_sltu: return render_rtype(insn, "sltu");
	case funct3_xor: return render_rtype(insn, "xor");
	case funct3_srx:
        switch (get_funct7(insn))
		{
		default: return render_illegal_insn(insn);
		case funct7_srl: return render_rtype(insn, "srl");
		case funct7_sra: return render_rtype(insn, "sra");
		}
    case funct3_or: return render_rtype(insn, "or");
	case funct3_and: return render_rtype(insn, "and");
    }
case opcode_system:
        switch (get_funct3(insn))
        {
        default: return render_illegal_insn(insn);

        case funct3_csrrw: return render_csrrx(insn, "csrrw");
        case funct3_csrrs: return render_csrrx(insn, "csrrs");
        case funct3_csrrc: return render_csrrx(insn, "csrrc");
        case funct3_csrrwi: return render_csrrxi(insn, "csrrwi");
        case funct3_csrrsi: return render_csrrxi(insn, "csrrsi");
        case funct3_csrrci: return render_csrrxi(insn, "csrrci");
        }
}
assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!	
}

/**
 * @brief extract and return opcode
 * 
 * @param insn instruction where opcode is extracted from
 * @return uint32_t = the opcode as 32bit unsigned int
 */
uint32_t rv32i_decode::get_opcode(uint32_t insn)
{
    return (insn & 0x0000007f);
}

/**
 * @brief extract and return rd from instruction
 * 
 * @param insn instruction where rd is extracted
 * @return uint32_t rd as 32bit unsigned int
 */
uint32_t rv32i_decode::get_rd(uint32_t insn)
{
    return (insn & 0x00000f80) >> 7; 
}

/**
 * @brief extract and return funct3 field
 * 
 * @param insn instruction to extract from
 * @return uint32_t funct3 as usinged 32 bit int
 */
uint32_t rv32i_decode::get_funct3(uint32_t insn)
{
    return (insn & 0x00007000) >> 12;
}

/**
 * @brief extract and return rs1 field
 * 
 * @param insn instruction where rs1 is extracted
 * @return uint32_t rs1 as usnigned 32bit int
 */
uint32_t rv32i_decode::get_rs1(uint32_t insn)
{
  return (insn & 0x000f8000) >> 15;
}

/**
 * @brief extract and return  rs2 field
 * 
 * @param insn instruction where rs2 is extracted
 * @return uint32_t rs2 as unsigned 32 bit int
 */
uint32_t rv32i_decode::get_rs2(uint32_t insn)
{
  return (insn & 0x01f00000) >> 20;
}

/**
 * @brief extract and return funct7 field
 * 
 * @param insn instruction where funct7 is extracted
 * @return uint32_t funct7 as unsigned 32bit int
 */
uint32_t rv32i_decode::get_funct7(uint32_t insn)
{
  return (insn & 0xfe000000) >> 25;
}

/**
 * @brief extract and return imm_i field
 * 
 * @param insn = instructuon to extract from
 * @return int32_t imm_i as signed 32 bit int
 */
int32_t rv32i_decode::get_imm_i(uint32_t insn)
{
    int32_t imm_i = (insn & 0xfff00000) >> 20;
    //sign extended
    if(imm_i & 0x800)
        imm_i |= 0xfffff000;
    return imm_i;
}

/**
 * @brief extract and return imm_u field
 * 
 * @param insn = instruction to extract from
 * @return int32_t imm_u as signed 32 bit int
 */
int32_t rv32i_decode::get_imm_u(uint32_t insn)
{
    //zero extened on right
    int32_t imm_u = insn & 0xfffff000;
    return imm_u;
}

/**
 * @brief extract and return imm_b field
 * 
 * @param insn = instruction to extract from
 * @return int32_t imm_b as signed 32 bit int
 */
int32_t rv32i_decode::get_imm_b(uint32_t insn)
{
    //zero extended on right
    int32_t imm_b = (insn & 0x80000000) >> (31-12);
    imm_b |= (insn & 0x7e000000) >> (25-5);
    imm_b |= (insn & 0x00000f00) >> (8-1);
    imm_b |= (insn & 0x00000080) << (11-7);

    //sign extend the left
    if(insn & 0x80000000)
        imm_b |= 0xffffe000;

    return imm_b;
}

/**
 * @brief extract and return imm_s field
 * 
 * @param insn = instruction
 * @return int32_t imm_s as signed 32 bit int
 */
int32_t rv32i_decode::get_imm_s(uint32_t insn)
{
    int32_t imm_s = (insn & 0xfe000000) >> (25-5);
    imm_s |= (insn & 0x00000f80) >> (7-0);

    if (insn & 0x80000000)
        imm_s |= 0xfffff000; // sign-extend the left

    return imm_s;
}

/**
 * @brief extract and return imm_j field
 * 
 * @param insn instruction where imm_j is extracted
 * @return int32_t imm_j as signed 32 bit int
 */
int32_t rv32i_decode::get_imm_j(uint32_t insn)
{
    int32_t imm_j = (insn & 0x80000000) >> (31-20);
    imm_j |= (insn & 0x7fe00000) >> (21-1);
    imm_j |= (insn & 0x00100000) >> (20-11);
    imm_j |= (insn & 0x000ff000);

    //sign extend left
    if(insn & 0x80000000)
        imm_j |= 0xffe00000;

    return imm_j;
}

/**
 * @brief print error message for invalid instructions
 * 
 * @param insn instruction that is illegal
 * @return std::string = message acknowledging the error
 */
std::string rv32i_decode::render_illegal_insn(uint32_t insn)
{
    return "ERROR: UNIMPLEMENTED INSTRUCTION";
}


/**
 * @brief format lui instruction as string
 * 
 * @param insn instruction that gets formatted to lui
 * @return std::string lui instruction stored for printing
 */
std::string rv32i_decode::render_lui(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    std::ostringstream os;
    os << render_mnemonic("lui") << render_reg(rd) << ","
        << to_hex0x20((imm_u >> 12)&0x0fffff);
    return os.str();
}

/**
 * @brief format auipc instruction as string
 * 
 * @param insn instruction that gets formatted to auipc
 * @return std::string auipc instruction stored for printing
 */
std::string rv32i_decode::render_auipc(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);
    std::ostringstream os;
    os << render_mnemonic("auipc") << render_reg(rd) << ","
        << to_hex0x20((imm_u >> 12)&0x0fffff);
    return os.str();
}

/**
 * @brief format jal instruction as string
 * 
 * @param addr used to calculate target address
 * @param insn instruction to get formatted to jal
 * @return std::string jal instruction stored for printing
 */
std::string rv32i_decode::render_jal(uint32_t addr, uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_j = get_imm_j(insn);
    std::ostringstream os;
    os << render_mnemonic("jal") << render_reg(rd) << ","
        << to_hex0x32(imm_j+addr);
    return os.str();
}

/**
 * @brief format jalr instruction as string
 * 
 * @param insn instruction to get formatted as jalr
 * @return std::string jalr isntruction stored for printing
 */
std::string rv32i_decode::render_jalr(uint32_t insn)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << render_mnemonic("jalr") << render_reg(rd) << ","
        << render_base_disp(rs1,imm_i);
    return os.str();
}

/**
 * @brief format imm_b type instructions
 * 
 * @param addr address of instruction to decode
 * @param insn instruction that will be decoded
 * @param mnemonic name of instruction
 * @return std::string imm_b format to be printed to stdout
 */
std::string rv32i_decode::render_btype(uint32_t addr, uint32_t insn, const char *mnemonic)
{
    int32_t imm_b = get_imm_b(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs1) << ","
        << render_reg(rs2) << "," << to_hex0x32(addr+imm_b);
    return os.str();
}

/**
 * @brief format imm_i type instructions
 * 
 * @param insn instruction that gets formatted
 * @param mnemonic name of instruction
 * @return std::string imm_i format to be printed to stdout
 */
std::string rv32i_decode::render_itype_load(uint32_t insn, const char *mnemonic)
{
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
        << render_base_disp(rs1,imm_i);
    return os.str();
}


/**
 * @brief format imm_s instructions
 * 
 * @param insn instruction to get decoded
 * @param mnemonic name of instruction
 * @return std::string imm_s format to be printed to stdout
 */
std::string rv32i_decode::render_stype(uint32_t insn, const char *mnemonic)
{
    int32_t imm_s = get_imm_s(insn);
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rs2) << ","
        << render_base_disp(rs1,imm_s);
    return os.str();
}

/**
 * @brief format i type alu instructions
 * 
 * @param insn instuction to decode
 * @param mnemonic name of instuction
 * @param imm_i immediate value
 * @return std::string in format of i type alu to print
 */
std::string rv32i_decode::render_itype_alu(uint32_t insn, const char *mnemonic, int32_t imm_i)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rd = get_rd(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
        << render_reg(rs1) << "," << imm_i;
    return os.str();
}

/**
 * @brief format rtype instructions
 * 
 * @param insn instruction to format
 * @param mnemonic name of instruction
 * @return std::string in format of rtype to print
 */
std::string rv32i_decode::render_rtype(uint32_t insn, const char *mnemonic)
{
    uint32_t rs1 = get_rs1(insn);
    uint32_t rs2 = get_rs2(insn);
    uint32_t rd = get_rd(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
        << render_reg(rs1) << "," << render_reg(rs2);
    return os.str();
}

/**
 * @brief return ecall
 * 
 * @param insn when instructions is "ecall"
 * @return std::string "ecall" stored as string
 */
std::string rv32i_decode::render_ecall(uint32_t insn)
{
    return "ecall";
}

/**
 * @brief return ebreak
 * 
 * @param insn when isntruction is "ebreak"
 * @return std::string "ebreak" stored into string
 */
std::string rv32i_decode::render_ebreak(uint32_t insn)
{
    return "ebreak";
}

/**
 * @brief decode csrrx isntructions
 * 
 * @param insn instructiom to decode
 * @param mnemonic name of instruction
 * @return std::string csrrx instruction to print
 */
std::string rv32i_decode::render_csrrx(uint32_t insn, const char *mnemonic)
{
    uint32_t csr = get_imm_i(insn);
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
        << to_hex0x12(csr) << "," << render_reg(rs1);
    return os.str();
}

/**
 * @brief decode csrrxi instructions
 * 
 * @param insn instruction to decode
 * @param mnemonic name of instruction
 * @return std::string csrrxi instruction to print
 */
std::string rv32i_decode::render_csrrxi(uint32_t insn, const char *mnemonic)
{
    uint32_t csr = get_imm_i(insn);
    uint32_t rd = get_rd(insn);
    uint32_t rs1 = get_rs1(insn);
    std::ostringstream os;
    os << render_mnemonic(mnemonic) << render_reg(rd) << ","
        << to_hex0x12(csr) << "," << rs1;
    return os.str();
}
/**
 * @brief render into a string name of register with given number
 * 
 * @param r = register
 * @return std::string the name of the register with "x" before number
 */
std::string rv32i_decode::render_reg(int r)
{
    std::ostringstream os;
    os << "x" << std::dec << r;
    return os.str();
}

/**
 * @brief set the base and displacement addresses
 * 
 * @param base address
 * @param disp displacement address
 * @return std::string printing out base displacement as string
 */
std::string rv32i_decode::render_base_disp(uint32_t base, int32_t disp)
{
    std::ostringstream os;
    os << std::dec << disp << "(" << render_reg(base) <<")";
    return os.str();
}

/**
 * @brief left justify instruction name with padding of mnemonic_width
 * 
 */
std::string rv32i_decode::render_mnemonic(const std::string &m)
{
    std::ostringstream os;
    os << std::left << std::setw(mnemonic_width) << m;
    return os.str();
}
