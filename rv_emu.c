#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rv_emu.h"
#include "bits.h"

#define DEBUG 0

static void unsupported(char *s, uint32_t n) {
    printf("unsupported %s 0x%x\n", s, n);
    exit(-1);
}

void rv_init(struct rv_state_st *rsp, uint32_t *func,
             uint64_t a0, uint64_t a1, uint64_t a2, uint64_t a3) {
    int i;

    // Zero out registers
    for (i = 0; i < RV_NREGS; i += 1) {
        rsp->regs[i] = 0;
    }

    // Zero out the stack
    for (i = 0; i < STACK_SIZE; i += 1) {
        rsp->stack[i] = 0;
    }

    // Initialize the Program Counter
    rsp->pc = (uint64_t) func;

    // Register 0 (zero) is always 0
    rsp->regs[RV_ZERO] = 0;

    // Initialize the Link Register to a sentinel value
    rsp->regs[RV_RA] = 0;

    // Initialize Stack Pointer to the logical bottom of the stack
    rsp->regs[RV_SP] = (uint64_t) &rsp->stack[STACK_SIZE];

    // Initialize the first 4 arguments in emulated r0-r3
    rsp->regs[RV_A0] = a0;
    rsp->regs[RV_A1] = a1;
    rsp->regs[RV_A2] = a2;
    rsp->regs[RV_A3] = a3;


    memset(&rsp->analysis, 0, sizeof(struct analysis_st));
    cache_init(&rsp->i_cache);
}

void emu_b_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	
	uint32_t imm12 = get_bits(iw, 31, 1);	// imm[12]
	uint32_t imm11 = get_bits(iw, 7, 1);	// imm[11]
	uint32_t imm10_5 = get_bits(iw, 25, 6);	// imm[10:5]
	uint32_t imm4_1 = get_bits(iw, 8, 4);	// imm[4:1]

	uint32_t imm32 = (imm12 << 12) | (imm11 << 11) | (imm10_5 << 5) | (imm4_1 << 1);	
	int64_t imm64 = sign_extend(imm32, 12);

	bool no_break = true;	
	if (funct3 == 0b000) {
		// beq
		if ((int64_t) rsp->regs[rs1] == (int64_t) rsp->regs[rs2]) {
			rsp->pc += imm64;
			rsp->analysis.b_taken += 1;
			no_break = false;
		}
	} else if (funct3 == 0b001) {
		// bne
		if ((int64_t) rsp->regs[rs1] != (int64_t) rsp->regs[rs2]) {
			rsp->pc += imm64;
			rsp->analysis.b_taken += 1;
			no_break = false;
		}
	} else if (funct3 == 0b100) {
		// blt & bgt
		if ((int64_t) rsp->regs[rs1] < (int64_t) rsp->regs[rs2]) {
			rsp->pc += imm64;
			rsp->analysis.b_taken += 1;
			no_break = false;
		}
	} else if (funct3 == 0b101) {
		// bge & ble
		if ((int64_t) rsp->regs[rs1] >= (int64_t) rsp->regs[rs2]) {
			rsp->pc += imm64;
			rsp->analysis.b_taken += 1;
			no_break = false;
		}
	} else {
		unsupported("B-type funct3", funct3);
	}
	if (no_break) {
		rsp->pc += 4;
		rsp->analysis.b_not_taken += 1;
	}
}

void emu_i_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);

	uint64_t imm11_0 = get_bits(iw, 20, 12);
	int64_t imm64 = sign_extend(imm11_0, 12);

	if (funct3 == 0b000) {
		// addi
		rsp->regs[rd] = rsp->regs[rs1] + imm64;
	} else if (funct3 == 0b001) {
		// slli
		rsp->regs[rd] = rsp->regs[rs1] << imm64;	
	} else if (funct3 == 0b101) {
		// srli
		rsp->regs[rd] = rsp->regs[rs1] >> imm64;
	} else if (funct3 == 0b101 && (imm11_0 >> 5) == 0b0100000) {
		// srai
		rsp->regs[rd] = (int64_t) rsp->regs[rs1] >> imm64;
	} else {
		unsupported("I-type funct3", funct3);
	}
	rsp->pc += 4;
}

// TODO note about i-type but load usage
void emu_l_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);

	uint64_t imm11_0 = get_bits(iw, 20, 12);
	int64_t imm64 = sign_extend(imm11_0, 12);

	if (funct3 == 0b000) {
		// lb
		rsp->regs[rd] = *((uint8_t *) (rsp->regs[rs1] + imm64));
	} else if (funct3 == 0b010) {
		// lw
		rsp->regs[rd] = *((uint32_t *) (uint64_t) (rsp->regs[rs1] + imm64));
	} else if (funct3 == 0b011) {
		// ld
		rsp->regs[rd] = *((uint64_t *) (uint64_t) (rsp->regs[rs1] + imm64));
	} else {
		unsupported("I-type (load) funct3", funct3);
	}
	rsp->pc += 4;
}

void emu_jalr(struct rv_state_st *rsp, uint32_t iw) {
    uint32_t rs1 = (iw >> 15) & 0b1111;
    uint64_t val = rsp->regs[rs1];

    rsp->pc = val;
}

void emu_j_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t imm20 = get_bits(iw, 31, 1);	
	uint32_t imm10_1 = get_bits(iw, 21, 10);	
	uint32_t imm11 = get_bits(iw, 20, 1);	
	uint32_t imm19_12 = get_bits(iw, 12, 8);	
	
	uint64_t imm21 = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);
	int64_t j = sign_extend(imm21, 20);
	
	if (rd != 0) {
		rsp->regs[rd] = rsp->pc + 4;	
	}
	rsp->pc += j;
}

void emu_r_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	uint32_t funct7 = get_bits(iw, 25, 7);

	if (funct3 == 0b000 && funct7 == 0b0000000) {
		// add
		rsp->regs[rd] = rsp->regs[rs1] + rsp->regs[rs2];
	} else if (funct3 == 0b000 && funct7 == 0b0100000) {
		// sub
		rsp->regs[rd] = rsp->regs[rs1] - rsp->regs[rs2];
	} else if (funct3 == 0b001 && funct7 == 0b0000000) {
		// sll
		rsp->regs[rd] = rsp->regs[rs1] << rsp->regs[rs2];
	} else if (funct3 == 0b101 && funct7 == 0b0000000) {
		// srl
		rsp->regs[rd] = rsp->regs[rs1] >> rsp->regs[rs2];
	} else if (funct3 == 0b101 && funct7 == 0b0100000) {
		// sra
		rsp->regs[rd] = (int64_t) rsp->regs[rs1] >> rsp->regs[rs2];
	} else if (funct3 == 0b000 && funct7 == 0b0000001) {
		// mul
		rsp->regs[rd] = rsp->regs[rs1] * rsp->regs[rs2];
	} else if (funct3 == 0b111 && funct7 == 0b0000000) {
		// and
		rsp->regs[rd] = rsp->regs[rs1] & rsp->regs[rs2];
	} else {
        unsupported("R-type funct3", funct3);
	}
	rsp->pc += 4;
}

void emu_r_shift(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rd = get_bits(iw, 7, 5);
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	uint32_t funct7 = get_bits(iw, 25, 7);
	
	if (funct3 == 0b001 && funct7 == 0b0000000) {
		// sllw
		rsp->regs[rd] = ((int32_t) rsp->regs[rs1] << (int32_t) rsp->regs[rs2]);
	} else if (funct3 == 0b101 && funct7 == 0b0000000) {
		// srlw
		rsp->regs[rd] = ((int32_t) rsp->regs[rs1] >> (int32_t) rsp->regs[rs2]);
	} else if (funct3 == 0b101 && funct7 == 0b0100000) {
		// sraw
		rsp->regs[rd] = ((int32_t) rsp->regs[rs1] >> (int32_t) rsp->regs[rs2]);
	} else {
		unsupported("R-type (shift word) funct3", funct3);
	}
	rsp->pc += 4;
}

void emu_s_type(struct rv_state_st *rsp, uint32_t iw) {
	uint32_t rs1 = get_bits(iw, 15, 5);
	uint32_t rs2 = get_bits(iw, 20, 5);
	uint32_t funct3 = get_bits(iw, 12, 3);
	
	uint32_t imm11_5 = get_bits(iw, 25, 7);	// imm[11:5]
	uint32_t imm4_0 = get_bits(iw, 7, 5);	// imm[4:0]

	uint32_t imm32 = (imm11_5 << 5) | imm4_0;	
	int64_t imm64 = sign_extend(imm32, 11);
	
	uint64_t addr = rsp->regs[rs1] + imm64;
	if (funct3 == 0b000) {
		// sb
		*((uint8_t *) addr) = rsp->regs[rs2] & 0xFF;
	} else if (funct3 == 0b010) {
		// sw
		*((uint32_t *) addr) = rsp->regs[rs2] & 0xFFFFFFFF;
	} else if (funct3 == 0b011) {
		// sd
		*((uint64_t *) addr) = rsp->regs[rs2];
	} else {
		unsupported("S-type funct3", funct3);
	}
	rsp->pc += 4;
}

static void rv_one(struct rv_state_st *rsp) {
	uint32_t iw = *(uint32_t*) rsp->pc;
	iw = cache_lookup(&rsp->i_cache, (uint64_t) rsp->pc);

#if DEBUG
    printf("iw: %08x\n", iw);
#endif

	uint32_t opcode = iw & 0b1111111;
	switch (opcode) {
		case FMT_B:
			emu_b_type(rsp, iw);
			break;
		case FMT_I_LOAD:
			emu_l_type(rsp, iw);
			rsp->analysis.ld_count += 1;
			break;
		case FMT_I_ARITH:
			emu_i_type(rsp, iw);
			rsp->analysis.ir_count += 1;
			break;
		case FMT_I_JALR:
			emu_jalr(rsp, iw);
			rsp->analysis.j_count += 1;
			break;
		case FMT_J:
			emu_j_type(rsp, iw);
			rsp->analysis.j_count += 1;
			break;
		case FMT_R:
			emu_r_type(rsp, iw);
			rsp->analysis.ir_count += 1;
			break;
		case FMT_R_SHIFT:
			emu_r_shift(rsp, iw);
			rsp->analysis.ir_count += 1;
			break;
		case FMT_S:
			emu_s_type(rsp, iw);
			rsp->analysis.st_count += 1;
			break;
		default:
			unsupported("Unknown opcode: ", opcode);  
	}
}

uint64_t rv_emulate(struct rv_state_st *rsp) {
    while (rsp->pc != RV_STOP) {
        rv_one(rsp);
		rsp->analysis.i_count += 1;
    }
    return rsp->regs[RV_A0];
}

static void print_pct(char *fmt, int numer, int denom) {
    double pct = 0.0;

    if (denom)
        pct = (double) numer / (double) denom * 100.0;
    printf(fmt, numer, pct);
}

void rv_print(struct analysis_st *ap) {
    int b_total = ap->b_taken + ap->b_not_taken;

    printf("=== Analysis\n");
    print_pct("Instructions Executed  = %d\n", ap->i_count, ap->i_count);
    print_pct("R-type + I-type        = %d (%.2f%%)\n", ap->ir_count, ap->i_count);
    print_pct("Loads                  = %d (%.2f%%)\n", ap->ld_count, ap->i_count);
    print_pct("Stores                 = %d (%.2f%%)\n", ap->st_count, ap->i_count);    
    print_pct("Jumps/JAL/JALR         = %d (%.2f%%)\n", ap->j_count, ap->i_count);
    print_pct("Conditional branches   = %d (%.2f%%)\n", b_total, ap->i_count);
    print_pct("  Branches taken       = %d (%.2f%%)\n", ap->b_taken, b_total);
    print_pct("  Branches not taken   = %d (%.2f%%)\n", ap->b_not_taken, b_total);
}
