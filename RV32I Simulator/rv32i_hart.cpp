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

#include "rv32i_hart.h"
#include "rv32i_decode.h"

#include <iostream>
#include <cassert>
#include <iomanip>

/**
 * @brief reset the rv32i object and the registerfile
 * 
 */
void rv32i_hart::reset()
{
    pc = 0;
    regs.reset();
    insn_counter = 0;
    halt = false;
    halt_reason = "none";
}

/**
 * @brief dump the entire state of the hart
 * 
 * @param hdr the prefix that is null or a string 
 */
void rv32i_hart::dump(const std::string &hdr) const 
{
    regs.dump(hdr);
    std::cout << std::endl << hdr << " pc " << to_hex32(pc) << std::endl;
}

/**
 * @brief tells the simulator how to execute instructions
 * 
 * @param hdr is printed before any output
 */
void rv32i_hart::tick(const std::string &hdr)
{
    if(halt == true)
        return;

    if(show_registers == true)
    {
        dump(hdr);
    } 

    if(pc & 0x03)
    {
        halt = true;
        halt_reason = " PC alignment error";
        return;
    }
    insn_counter++;
    uint32_t insn = mem.get32(pc);
    if(show_instructions)
    {
        std::cout << hdr << hex::to_hex32(pc) << ": " << hex::to_hex32(insn) << "  ";
        exec(insn, &std::cout);
        std::cout << std::endl;
    }
    else
    {
        exec(insn, nullptr);
    } 

}

/**
 * @brief execute the given instruction depending on what it is
 * 
 * @param insn the instruction name 
 */
void rv32i_hart::exec(uint32_t insn,std::ostream *pos)
{

switch(insn)
{  
case insn_ecall: exec_ecall(insn, pos); return;     
case insn_ebreak: exec_ebreak(insn, pos); return;   
} 

switch(get_opcode(insn))
{
default: exec_illegal_insn(insn,pos); return;
case opcode_lui: exec_lui(insn,pos); return;
case opcode_auipc: exec_auipc(insn,pos); return;
case opcode_jal: exec_jal(insn,pos); return;
case opcode_jalr: exec_jalr(insn,pos); return;
case opcode_btype:
    switch(get_funct3(insn))
    {
    default: exec_illegal_insn(insn,pos); return;
    case funct3_beq: exec_beq(insn,pos); return;
    case funct3_bne: exec_bne(insn,pos); return;
    case funct3_blt: exec_blt(insn,pos); return;
    case funct3_bge: exec_bge(insn,pos); return;
    case funct3_bltu: exec_bltu(insn,pos); return;
    case funct3_bgeu: exec_bgeu(insn,pos); return;
    }
    assert(0 && "unrecognized funct3"); // impossible
case opcode_alu_imm:
    switch(get_funct3(insn))
    {
    default: exec_illegal_insn(insn,pos); return;
    case funct3_add: exec_addi(insn,pos); return;
    case funct3_sll: exec_slli(insn,pos); return;
    case funct3_slt: exec_slti(insn,pos); return;
    case funct3_sltu: exec_sltiu(insn,pos); return;
    case funct3_xor: exec_xori(insn,pos); return;
    case funct3_or: exec_ori(insn,pos); return;
    case funct3_and: exec_andi(insn,pos); return;
    case funct3_srx:
        switch(get_funct7(insn))
        {
        default: exec_illegal_insn(insn,pos); return;
        case funct7_sra: exec_srai(insn,pos); return;
        case funct7_srl: exec_srli(insn,pos); return;
        }
        assert(0 && "unrecognized funct7"); // impossible 
    }
    assert(0 && "unrecognized funct3"); // impossible
case opcode_load_imm:
    switch (get_funct3(insn))
    {
    default: exec_illegal_insn(insn,pos); return;
    case funct3_lb:	exec_lb(insn,pos); return;
    case funct3_lh:	exec_lh(insn,pos); return;
    case funct3_lw: exec_lw(insn,pos); return;
    case funct3_lbu: exec_lbu(insn,pos); return;
    case funct3_lhu: exec_lhu(insn,pos); return;
    }
case opcode_stype:
    switch (get_funct3(insn))
    {
    default: exec_illegal_insn(insn,pos); return;
    case funct3_sb:	exec_sb(insn,pos); return;
    case funct3_sh:	exec_sh(insn,pos); return;
    case funct3_sw: exec_sw(insn,pos); return;
    }
case opcode_rtype:
    switch(get_funct3(insn))
    {
    default: exec_illegal_insn(insn,pos); return;
    case funct3_add:
        switch(get_funct7(insn))
        {
        default: exec_illegal_insn(insn,pos); return;
        case funct7_add: exec_add(insn,pos); return;
        case funct7_sub: exec_sub(insn,pos); return;
        }
    case funct3_sll: exec_sll(insn,pos); return;
	case funct3_slt: exec_slt(insn,pos); return;
	case funct3_sltu: exec_sltu(insn,pos); return;
	case funct3_xor: exec_xor(insn,pos); return;
	case funct3_srx:
        switch (get_funct7(insn))
		{
		default: exec_illegal_insn(insn,pos); return;
		case funct7_srl: exec_srl(insn,pos); return;
		case funct7_sra: exec_sra(insn,pos); return;
		}
    case funct3_or: exec_or(insn,pos); return;
	case funct3_and: exec_and(insn,pos); return;
    }
case opcode_system:
        switch (get_funct3(insn))
        {
        default: exec_illegal_insn(insn,pos); return;

        case funct3_csrrs: exec_csrrs(insn,pos); return;
               
        }
}
assert(0 && "unrecognized opcode"); // It should be impossible to ever get here!
}

/**
 * @brief print proper message if given instruction is illegal
 * 
 * @param insn instruction name
 * @param pos variable storing illegal instruction name
 */
void rv32i_hart::exec_illegal_insn(uint32_t insn, std::ostream* pos)
{
    if(pos)
        *pos << render_illegal_insn(insn);
    halt = true;
    halt_reason = "Illegal instruction";
}

/**
 * @brief simulate execution of lui
 * 
 * @param insn instruction to execute
 * @param pos where lui message is stored
 */
void rv32i_hart::exec_lui(uint32_t insn, std::ostream *pos)
{
    uint32_t rd = get_rd(insn);
    int32_t imm_u = get_imm_u(insn);

    if(pos)
    {
        std::string s = render_lui(insn);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(imm_u) ; 
    }
    regs.set(rd,imm_u);
    pc += 4;
}

/**
 * @brief simulate execution of auipc
 * 
 * @param insn instruction to execute
 * @param pos where auipc message is stored
 */
void rv32i_hart::exec_auipc(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn);
	int32_t imm_u = get_imm_u(insn);

	if (pos)
	{
		std::string s = render_auipc(insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_u) << " = " << hex::to_hex0x32(pc + imm_u);
	}
	regs.set(rd, (imm_u+pc));
	pc += 4;
}

/**
 * @brief simulate execution of jal
 * 
 * @param insn instruction to execute
 * @param pos where jal message is stored
 */
void rv32i_hart::exec_jal(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	int32_t imm_j = get_imm_j(insn);
	int32_t val = pc + imm_j;

	if(pos)
	{
		std::string s = render_jal(pc, insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc + 4) << ",  pc = " << hex::
			to_hex0x32(pc) << " + " << hex::to_hex0x32(imm_j) << " = " << hex::to_hex0x32(val); 
	}
	regs.set(rd, pc + 4);
	pc = val;
}

/**
 * @brief simulate execution of jalr
 * 
 * @param insn instruction to execute
 * @param pos where jalr message is stored
 */
void rv32i_hart::exec_jalr(uint32_t insn, std::ostream* pos) 
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t imm_i = get_imm_i(insn);
	int32_t temp = pc + 4;

	if (pos)
	{
		std::string s = render_jalr(insn);
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(pc + 4) << ",  pc = (" << hex::to_hex0x32(imm_i) << " + " << hex::to_hex0x32(regs.get(rs1)) << ") & 0xfffffffe = " << hex::to_hex0x32((regs.get(rs1) + imm_i)&0xfffffffe );
	}
	pc = ((imm_i+regs.get(rs1))&0xfffffffe);
	regs.set(rd, temp);

}

/**
 * @brief simulate execution of beq
 * 
 * @param insn instruction to execute
 * @param pos where beq message is stored
 */
void rv32i_hart::exec_beq(uint32_t insn, std::ostream* pos)
{
    uint32_t rs1 = get_rs1(insn); 
    uint32_t rs2 = get_rs2(insn); 
    uint32_t imm_b = get_imm_b(insn);

    int32_t val;
    if(regs.get(rs1) == regs.get(rs2))
    {
        val = pc + imm_b;
    }
    else val = pc + 4;

    if(pos)
    {
        std::string s = render_btype(pc, insn, "beq"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " == " << hex::to_hex0x32(regs.get(rs2)) << 
        " ? " << hex::to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }
    pc = val; 
}

/**
 * @brief simulate execution for bne
 * 
 * @param insn instructino to execute
 * @param pos where bne message is stored
 */
void rv32i_hart::exec_bne(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t val;

    if(regs.get(rs1) != regs.get(rs2))
    {
      	val = pc + imm_b;
    }
    else val = pc + 4;
    
    if(pos)
    {
	    std::string s = render_btype(pc, insn, "bne");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
	    *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " != " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::
        to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val); 
    }
    pc = val; 
}

/**
 * @brief simulate execution of blt
 * 
 * @param insn instruction to execute
 * @param pos where blt message gets stored
 */
void rv32i_hart::exec_blt(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t val;

    if(regs.get(rs1) < regs.get(rs2))
    {
      	val = pc + imm_b;
    }
    else val = pc + 4;
    
    if(pos)
    {
        std::string s = render_btype(pc, insn, "blt");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
	    *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " < " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::
        to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }
    pc = val;  
}

/**
 * @brief simulate execution of bge
 * 
 * @param insn instruction to execute
 * @param pos where bge message is stored
 */
void rv32i_hart::exec_bge(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t val;

    if(regs.get(rs1) >= regs.get(rs2))
    {
      	val = pc + imm_b;
    }
    else val = pc + 4;
    
    if(pos)
    {
        std::string s = render_btype(pc, insn, "bge");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
	    *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " >= " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::
        to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }
    pc = val;
}

/**
 * @brief simulate execution of bltu
 * 
 * @param insn instuction to execute
 * @param pos where bltu message is stored 
 */
void rv32i_hart::exec_bltu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t val;

    if((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2))
    {
        val = pc + imm_b;
    }
    else val = pc + 4;
    
    if(pos)
    {
        std::string s = render_btype(pc, insn, "bltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " <U " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::
        to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }
    pc = val;
}

/**
 * @brief simulate execution of bgeu
 * 
 * @param insn instruction to execute
 * @param pos where bgeu message is stored
 */
void rv32i_hart::exec_bgeu(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_b = get_imm_b(insn);
    int32_t val;

    if((uint32_t)regs.get(rs1) >= (uint32_t)regs.get(rs2))
    {
      	val = pc + imm_b;
    }
    else val = pc + 4;

    if(pos)
    {
        std::string s = render_btype(pc, insn, "bgeu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// pc += (" << hex::to_hex0x32(regs.get(rs1)) << " >=U " << hex::to_hex0x32(regs.get(rs2)) << " ? " << hex::
        to_hex0x32(imm_b) << " : 4) = " << hex::to_hex0x32(val);
    }
    pc = val;
}

/**
 * @brief simulate execution of lb
 * 
 * @param insn instruction to execute
 * @param pos where lb message is stored
 */
void rv32i_hart::exec_lb(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t addr = regs.get(rs1) + imm_i;
    uint32_t val = mem.get8(addr);
    
    if(val & 0x00000080)
    {
        val += 0xffffff00;
    }

    if(pos)
    {
        std::string s = render_itype_load(insn, "lb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::
        to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate execution of lh
 * 
 * @param insn instruction to execute
 * @param pos where lh message is stored
 */
void rv32i_hart::exec_lh(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t addr = regs.get(rs1) + imm_i;
    uint32_t val = mem.get16(addr);

    if(val & 0x00008000)
    {
        val += 0xffff0000;
    }

    if(pos)
    {
        std::string s = render_itype_load(insn, "lh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::
		to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate lw execution
 * 
 * @param insn instruction to execute
 * @param pos where lw message is stored
 */
void rv32i_hart::exec_lw(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rd = get_rd(insn);
    int32_t imm_i = get_imm_i(insn);
    uint32_t addr = regs.get(rs1) + imm_i;
    uint32_t val = mem.get32(addr);

    if(val & 0x80000000)
    {
        val += 0;
    }

    if(pos)
    {
        std::string s = render_itype_load(insn, "lw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = sx(m32(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::
		    to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate lbu execution
 * 
 * @param insn intruction to execute
 * @param pos where lbu message is stored
 */
void rv32i_hart::exec_lbu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn);
    uint32_t imm_i = get_imm_i(insn);

    int32_t addr = regs.get(rs1) + imm_i;
    uint32_t val = mem.get8(addr); 

    if (pos)
    {
        std::string s = render_itype_load(insn, "lbu"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = zx(m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " <<
        hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); 
    pc += 4; 
}

/**
 * @brief simulate lhu execution
 * 
 * @param insn intruction to execute
 * @param pos where lhu message is stored
 */
void rv32i_hart::exec_lhu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    uint32_t imm_i = get_imm_i(insn); 
    int32_t addr = regs.get(rs1) + imm_i;
    uint32_t val = mem.get16(addr); 

    if(pos)
    {
        std::string s = render_itype_load(insn, "lhu"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = zx(m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << 
        hex::to_hex0x32(imm_i) << ")) = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); 
    pc += 4; 
}

/**
 * @brief simulate sb execution
 * 
 * @param insn instruction to execute
 * @param pos where sb message is stored
 */
void rv32i_hart::exec_sb(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);
    uint32_t addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2) & 0x000000ff;

    if(pos)
    {
        std::string s = render_stype(insn, "sb");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m8(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::
		    to_hex0x32(val);
    }
    mem.set8(addr, val);
    pc += 4;
}

/**
 * @brief simulate sh execution
 * 
 * @param insn instructino to execute
 * @param pos where sh message iss stored
 */
void rv32i_hart::exec_sh(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn); 
    uint32_t addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2) & 0x0000ffff;

    if(pos)
    {
        std::string s = render_stype(insn, "sh");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m16(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::
		to_hex0x32(val);
    }
    mem.set16(addr, val);
    pc += 4;
}

/**
 * @brief simulate sw execution
 * 
 * @param insn instruction to execute
 * @param pos where sw message is stored
 */
void rv32i_hart::exec_sw(uint32_t insn, std::ostream* pos)
{
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t imm_s = get_imm_s(insn);
    uint32_t addr = regs.get(rs1) + imm_s;
    uint32_t val = regs.get(rs2);

    if(pos)
    {
        std::string s = render_stype(insn, "sw");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// m32(" << hex::to_hex0x32(regs.get(rs1)) << " + " << hex::to_hex0x32(imm_s) << ") = " << hex::
		to_hex0x32(val);
    }
    mem.set32(addr, val);
    pc += 4;
}

/**
 * @brief simulate addi execution
 * 
 * @param insn instruction to execute
 * @param pos where addi message is stored
 */
void rv32i_hart::exec_addi(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    uint32_t imm_i = get_imm_i(insn); 
    int32_t val = regs.get(rs1) + imm_i; 

    if (pos)
    {
        std::string s = render_itype_alu(insn, "addi", imm_i); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " + " << 
        hex::to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); 
    pc += 4; 

}

/**
 * @brief simulate slti execution
 * 
 * @param insn instruction to execute
 * @param pos where slti message iss stored
 */
void rv32i_hart::exec_slti(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t val = (regs.get(rs1) < imm_i) ? 1 : 0;

    if(pos)
    {
	    std::string s = render_itype_alu(insn, "slti", imm_i);
	    *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
	    *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " < " << 
        std::dec << imm_i << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate sltiu execution
 * 
 * @param insn instuction to execute
 * @param pos where sltiu message is stored
 */
void rv32i_hart::exec_sltiu(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    int32_t imm_i = get_imm_i(insn); 
    uint32_t val = ((static_cast<uint32_t>(regs.get(rs1))) < (static_cast<uint32_t>(imm_i))) ? 1 : 0; 

    if (pos)
    {
        std::string s = render_itype_alu(insn, "sltiu", imm_i); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " <U " << 
        std::dec << imm_i << ") ? 1 : 0 = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); 
    pc += 4; 
}

/**
 * @brief simulate xori execution
 * 
 * @param insn instuction to execute
 * @param pos where xori message is stored
 */
void rv32i_hart::exec_xori(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t val = regs.get(rs1) ^ imm_i;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "xori", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " ^ " << hex::
		to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate execution of ori
 * 
 * @param insn insttuction to execute
 * @param pos where message for ori is stored
 */
void rv32i_hart::exec_ori(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t val = regs.get(rs1) | imm_i;

    if(pos)
    {
       	std::string s = render_itype_alu(insn, "ori", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " | " << hex::
		to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate andi execution
 * 
 * @param insn instuction  to execute
 * @param pos where andi message is stored
 */
void rv32i_hart::exec_andi(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t imm_i = get_imm_i(insn);
    int32_t val = (regs.get(rs1) & imm_i);

    if(pos)
    {
        std::string s = render_itype_alu(insn, "andi", imm_i);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " & " << hex::
		to_hex0x32(imm_i) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate slli execution
 * 
 * @param insn instruction to execute
 * @param pos where slli message is stored
 */
void rv32i_hart::exec_slli(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    int32_t shamt_i = get_rs2(insn); 
    int32_t val = regs.get(rs1) << shamt_i; 

    if (pos)
    {
        std::string s = render_itype_alu(insn, "slli", shamt_i); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " << " << 
        std::dec << shamt_i << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val); 
    pc += 4; 
}

/**
 * @brief simulation for srli execution
 * 
 * @param insn instruction to execute
 * @param pos where srli message is stored
 */
void rv32i_hart::exec_srli(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t shift = get_imm_i(insn)%XLEN;
    int32_t val = (uint32_t)regs.get(rs1) >> shift;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "srli", get_imm_i(insn));
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << 
		std::dec << shift << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulation for srai execution 
 * 
 * @param insn instruction to execute
 * @param pos where srai message is stored
 */
void rv32i_hart::exec_srai(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t shift = get_imm_i(insn)%XLEN;

    int32_t val = regs.get(rs1) >> shift;

    if(pos)
    {
        std::string s = render_itype_alu(insn, "srai", (int32_t)get_imm_i(insn)%XLEN);
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << 
		std::dec << shift << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate add execution
 * 
 * @param insn instruction to execute
 * @param pos where add message gets stored
 */
void rv32i_hart::exec_add(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    uint32_t rs2 = get_rs2(insn); 
    int32_t val = regs.get(rs1) + regs.get(rs2); 

    if (pos)
    {
        std::string s = render_rtype(insn, "add"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " + " << 
        hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); 
    pc += 4; 
}

/**
 * @brief simulate sub execution
 * 
 * @param insn instruction to execute
 * @param pos where sub message iss stored
 */
void rv32i_hart::exec_sub(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    uint32_t rs2 = get_rs2(insn);
    int32_t val = regs.get(rs1) - regs.get(rs2); 

    if (pos)
    {
        std::string s = render_rtype(insn, "sub"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " - " << 
        hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val); //set register to val
    pc += 4; //increment pc by 4
}

/**
 * @brief simulate sll execution
 * 
 * @param insn instruction to execute
 * @param pos where sll message is stored
 */
void rv32i_hart::exec_sll(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn); 
    int32_t val = regs.get(rs1) << (regs.get(rs2) % XLEN); 

    if (pos)
    {
        std::string s = render_rtype(insn, "sll");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " << " << 
        std::dec << (regs.get(rs2) % XLEN) << " = " << hex::to_hex0x32(val);
    }

    regs.set(rd, val); //set register to val
    pc += 4; //increment pc by 4
}

/**
 * @brief simulate execution of slt
 * 
 * @param insn instruction to execute
 * @param pos where slt message is stored
 */
void rv32i_hart::exec_slt( uint32_t insn , std :: ostream * pos)
{
    uint32_t rd = get_rd( insn );
    uint32_t rs1 = get_rs1( insn );
    uint32_t rs2 = get_rs2( insn );
    int32_t val = (regs.get(rs1) < regs.get(rs2)) ? 1 : 0;

    if(pos)
    {
        std::string s = render_rtype(insn,"slt ");
        *pos << std::setw(instruction_width) << std::setfill (' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32 (regs.get(rs1)) << " < " << hex::
        to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32( val );
    }
    regs.set(rd , val );
    pc += 4;
 }

/**
 * @brief simulate sltu execution
 * 
 * @param insn instruction to execute
 * @param pos where sltu message is stored
 */
void rv32i_hart::exec_sltu(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t val = ((uint32_t)regs.get(rs1) < (uint32_t)regs.get(rs2)) ? 1 : 0; 

    if(pos)
    {
        std::string s = render_rtype(insn, "sltu");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = (" << hex::to_hex0x32(regs.get(rs1)) << " <U " << hex::
		to_hex0x32(regs.get(rs2)) << ") ? 1 : 0 = " << hex::to_hex0x32(val); 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate xor execution
 * 
 * @param insn instruction to execute
 * @param pos where xor message iss stored
 */
void rv32i_hart::exec_xor(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t val = regs.get(rs1) ^ regs.get(rs2); 

    if(pos)
    {
        std::string s = render_rtype(insn, "xor");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " ^ " << hex::
		to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val); 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate srl execution
 * 
 * @param insn instruction to execute
 * @param pos where srl message is stored
 */
void rv32i_hart::exec_srl(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t shift = regs.get(rs2) & 0x0000001f;
    int32_t val = (uint32_t)regs.get(rs1) >> shift; 

    if(pos)
    {
        std::string s = render_rtype(insn, "srl");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << 
		std::dec << shift << " = " << hex::to_hex0x32(val); 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate sra execution
 * 
 * @param insn instruction to execute 
 * @param pos where sra message is stored
 */
void rv32i_hart::exec_sra(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t shift = regs.get(rs2) & 0x0000001f;
    int32_t val = regs.get(rs1) >> shift; 

    if(pos)
    {
        std::string s = render_rtype(insn, "sra");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " >> " << 
		std::dec << shift << " = " << hex::to_hex0x32(val); 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate or execution
 * 
 * @param insn instruction to execute
 * @param pos where or message is stored
 */
void rv32i_hart::exec_or(uint32_t insn, std::ostream* pos)
{
    uint32_t rd = get_rd(insn); 
    uint32_t rs1 = get_rs1(insn); 
    uint32_t rs2 = get_rs2(insn); 
    int32_t val = regs.get(rs1) | regs.get(rs2); 

    if (pos)
    {
        std::string s = render_rtype(insn, "or"); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " | " << 
        hex::to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val);
    }
    regs.set(rd, val); //set register to value
    pc += 4; //increment pc by 4
}

/**
 * @brief simulate and execution
 * 
 * @param insn instruction to execute
 * @param pos where and  message is stored
 */
void rv32i_hart::exec_and(uint32_t insn, std::ostream* pos)
{
    int32_t rd = get_rd(insn);
    int32_t rs1 = get_rs1(insn);
    int32_t rs2 = get_rs2(insn);
    int32_t val = regs.get(rs1) & regs.get(rs2); 

    if(pos)
    {
        std::string s = render_rtype(insn, "and");
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
        *pos << "// " << render_reg(rd) << " = " << hex::to_hex0x32(regs.get(rs1)) << " & " << hex::
		to_hex0x32(regs.get(rs2)) << " = " << hex::to_hex0x32(val); 
    }
    regs.set(rd, val);
    pc += 4;
}

/**
 * @brief simulate ecall execution
 * 
 * @param insn instructino to execute
 * @param pos where ecall message is stored
 */
void rv32i_hart::exec_ecall(uint32_t insn, std::ostream* pos)
{
    if (pos)
    {
        std::string s = render_ecall(insn); 
        *pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s; 
        *pos << "// HALT"; 
        std::cout<<std::endl;
    }

    halt = true; 
    halt_reason = "ECALL instruction"; 
}

/**
 * @brief simulate ebreak execution
 * 
 * @param insn instruction to execute
 * @param pos where ebreak message iss stored
 */
void rv32i_hart::exec_ebreak( uint32_t insn , std :: ostream * pos )
{
    if ( pos )
    {
        std::string s = render_ebreak( insn );
        *pos << std::setw( instruction_width ) << std::setfill (' ') << std::left << s;
        * pos << "// HALT"; 
    }
    halt = true ;
    halt_reason = " EBREAK instruction";
}

/**
 * @brief simulate csrrs execution
 * 
 * @param insn instruction to execute
 * @param pos where csrrs message is stored
 */
void rv32i_hart::exec_csrrs(uint32_t insn, std::ostream* pos)
{
	uint32_t rd = get_rd(insn);
	uint32_t rs1 = get_rs1(insn);
	int32_t csr = get_imm_i(insn) & 0x00000fff;

	if(csr != 0xf14 || rs1 != 0)
	{
		halt = true;
		halt_reason = "Illegal CSR in CRSS instruction";
	}
	
	if(pos)
	{
		std::string s = render_csrrx(insn, "csrrs");
		*pos << std::setw(instruction_width) << std::setfill(' ') << std::left << s;
		*pos << "// " << render_reg(rd) << " = " << std::dec << mhartid;
	}

	if(!halt)
	{
		regs.set(rd, mhartid);
		pc += 4;
	}
}

