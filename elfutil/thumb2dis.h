// thumb2dis.h

#pragma once

#include <cstdint>
#include <string>
#include <iostream>
#include <cstdio>

std::string get_reg_name(int r) {
	switch (r) {
		case 13: return "SP";
		case 14: return "LR";
		case 15: return "PC";
		default: return "R" + std::to_string(r);
	}
}

std::string format_reg_list(int rlist) {
	std::string s = "{";
	bool first = true;
	for (int i = 0; i < 16; i++) {
		if (rlist & (1 << i)) {
				if (!first) s += ", ";
				s += get_reg_name(i);
				first = false;
		}
	}
	s += "}";
	return s;
}

std::string disasm_thumb2_16bit(uint16_t code, uint32_t addr) {
	int rd = code & 7;
	int rs = (code >> 3) & 7;
	int ro = (code >> 6) & 7;
	int imm5 = (code >> 6) & 0x1f;
	int imm8 = code & 0xff;
	int d4 = (code & 7) | (((code >> 7) & 1) << 3);
	int s4 = (code >> 3) & 7;
	char buf[128];

	// T1
	if ((code & 0xF800) == 0x0000) return "LSLS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #" + std::to_string(imm5);
	if ((code & 0xF800) == 0x0800) return "LSRS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #" + std::to_string(imm5 ? imm5 : 32);
	if ((code & 0xF800) == 0x1000) return "ASRS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #" + std::to_string(imm5 ? imm5 : 32);

	// T2
	if ((code & 0xFE00) == 0x1800) return "ADDS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", " + get_reg_name(ro);
	if ((code & 0xFE00) == 0x1A00) return "SUBS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", " + get_reg_name(ro);
	if ((code & 0xFE00) == 0x1C00) return "ADDS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #" + std::to_string(imm5);
	if ((code & 0xFE00) == 0x1E00) return "SUBS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #" + std::to_string(imm5);

	// T3
	if ((code & 0xF800) == 0x2000) return "MOVS " + get_reg_name(rd) + ", #" + std::to_string(imm8);
	if ((code & 0xF800) == 0x2800) return "CMP " + get_reg_name(rd) + ", #" + std::to_string(imm8);
	if ((code & 0xF800) == 0x3000) return "ADDS " + get_reg_name(rd) + ", #" + std::to_string(imm8);
	if ((code & 0xF800) == 0x3800) return "SUBS " + get_reg_name(rd) + ", #" + std::to_string(imm8);

	// T4 ALU
	if ((code & 0xFFC0) == 0x4000) return "ANDS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4040) return "EORS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4080) return "LSLS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x40C0) return "LSRS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4100) return "ASRS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4140) return "ADCS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4180) return "SBCS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x41C0) return "RORS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4200) return "TST " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4240) return "RSBS " + get_reg_name(rd) + ", " + get_reg_name(rs) + ", #0";
	if ((code & 0xFFC0) == 0x4280) return "CMP " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x42C0) return "CMN " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4300) return "ORRS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4340) return "MULS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x4380) return "BICS " + get_reg_name(rd) + ", " + get_reg_name(rs);
	if ((code & 0xFFC0) == 0x43C0) return "MVNS " + get_reg_name(rd) + ", " + get_reg_name(rs);

	// T5
	if ((code & 0xFF00) == 0x4400) return "ADD " + get_reg_name(d4) + ", " + get_reg_name(s4);
	if ((code & 0xFF00) == 0x4500) return "CMP " + get_reg_name(d4) + ", " + get_reg_name(s4);
	if ((code & 0xFF00) == 0x4600) return "MOV " + get_reg_name(d4) + ", " + get_reg_name(s4);
	if ((code & 0xFF00) == 0x4700) return "BX " + get_reg_name(s4);
	if ((code & 0xFF00) == 0x4780) return "BLX " + get_reg_name(s4);

	// T6 LDR literal
	if ((code & 0xF800) == 0x4800) {
		uint32_t target = (addr + 4) & ~3;
		target += imm8 * 4;
		snprintf(buf, sizeof(buf), "LDR %s, [0x%X]", get_reg_name(rd).c_str(), target);
		return buf;
	}

	// T7/T8 Load/Store reg
	if ((code & 0xFE00) == 0x5000) return "STR " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5200) return "STRH " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5400) return "STRB " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5600) return "LDSB " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5800) return "LDR " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5A00) return "LDRH " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5C00) return "LDRB " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";
	if ((code & 0xFE00) == 0x5E00) return "LDSH " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", " + get_reg_name(ro) + "]";

	// T9 Load/Store imm
	if ((code & 0xF800) == 0x6000) return "STR " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5*4) + "]";
	if ((code & 0xF800) == 0x6800) return "LDR " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5*4) + "]";
	if ((code & 0xF800) == 0x7000) return "STRB " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5) + "]";
	if ((code & 0xF800) == 0x7800) return "LDRB " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5) + "]";

	// T10 STRH/LDRH imm
	if ((code & 0xF800) == 0x8000) return "STRH " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5*2) + "]";
	if ((code & 0xF800) == 0x8800) return "LDRH " + get_reg_name(rd) + ", [" + get_reg_name(rs) + ", #" + std::to_string(imm5*2) + "]";

	// T11 SP rel
	if ((code & 0xF800) == 0x9000) return "STR " + get_reg_name(rd) + ", [SP, #" + std::to_string(imm8*4) + "]";
	if ((code & 0xF800) == 0x9800) return "LDR " + get_reg_name(rd) + ", [SP, #" + std::to_string(imm8*4) + "]";

	// T12 ADR / ADD SP
	if ((code & 0xF800) == 0xA000) {
		uint32_t target = (addr + 4) & ~3;
		target += imm8 * 4;
		snprintf(buf, sizeof(buf), "ADR %s, 0x%X", get_reg_name(rd).c_str(), target);
		return buf;
	}
	if ((code & 0xF800) == 0xA800) return "ADD " + get_reg_name(rd) + ", SP, #" + std::to_string(imm8*4);

	// T13 ADD/SUB SP
	if ((code & 0xFF80) == 0xB000) return "ADD SP, SP, #" + std::to_string((code & 0x7f) * 4);
	if ((code & 0xFF80) == 0xB080) return "SUB SP, SP, #" + std::to_string((code & 0x7f) * 4);

	// T14 PUSH/POP
	if ((code & 0xFE00) == 0xB400) {
		int rlist = code & 0xFF;
		if (code & 0x100) rlist |= (1 << 14);
		return "PUSH " + format_reg_list(rlist);
	}
	if ((code & 0xFE00) == 0xBC00) {
		int rlist = code & 0xFF;
		if (code & 0x100) rlist |= (1 << 15);
		return "POP " + format_reg_list(rlist);
	}

	// T15 STMIA/LDMIA
	if ((code & 0xF800) == 0xC000) return "STMIA " + get_reg_name(rd) + "!, " + format_reg_list(code & 0xFF);
	if ((code & 0xF800) == 0xC800) return "LDMIA " + get_reg_name(rd) + "!, " + format_reg_list(code & 0xFF);

	// T16 Bcc
	if ((code & 0xF800) == 0xD000) {
		const char* cc[] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","AL","NV"};
		int cond = (code >> 8) & 0xf;
		int off = imm8;
		if (off & 0x80) off -= 256;
		off *= 2;
		snprintf(buf, sizeof(buf), "B%s 0x%X", cc[cond], addr + 4 + off);
		return buf;
	}

	// T17 SVC
	if ((code & 0xFF00) == 0xDF00) return "SVC #" + std::to_string(imm8);

	// T18 B
	if ((code & 0xF800) == 0xE000) {
		int off = code & 0x7ff;
		if (off & 0x400) off -= 0x800;
		off *= 2;
		snprintf(buf, sizeof(buf), "B 0x%X", addr + 4 + off);
		return buf;
	}

	snprintf(buf, sizeof(buf), "DCW 0x%04X", code);
	return buf;
	}

	std::string disasm_32bit(uint16_t code1, uint16_t code2, uint32_t addr) {
	char buf[128];

	// BL
	if ((code1 & 0xF800) == 0xF000 && (code2 & 0xD000) == 0xD000) {
		uint32_t s = (code1 >> 10) & 1;
		uint32_t j1 = (code2 >> 13) & 1;
		uint32_t j2 = (code2 >> 11) & 1;
		uint32_t i1 = ~(j1 ^ s) & 1;
		uint32_t i2 = ~(j2 ^ s) & 1;
		uint32_t imm10 = code1 & 0x3ff;
		uint32_t imm11 = code2 & 0x7ff;
		int32_t off = (s << 24) | (i1 << 23) | (i2 << 22) | (imm10 << 12) | (imm11 << 1);
		if (s) off |= 0xFE000000; // sign extend
		snprintf(buf, sizeof(buf), "BL 0x%X", addr + 4 + off);
		return buf;
	}

	// B.W
	if ((code1 & 0xF800) == 0xF000 && (code2 & 0xD000) == 0x9000) {
		uint32_t s = (code1 >> 10) & 1;
		uint32_t j1 = (code2 >> 13) & 1;
		uint32_t j2 = (code2 >> 11) & 1;
		uint32_t i1 = ~(j1 ^ s) & 1;
		uint32_t i2 = ~(j2 ^ s) & 1;
		uint32_t imm10 = code1 & 0x3ff;
		uint32_t imm11 = code2 & 0x7ff;
		int32_t off = (s << 24) | (i1 << 23) | (i2 << 22) | (imm10 << 12) | (imm11 << 1);
		if (s) off |= 0xFE000000; // sign extend
		snprintf(buf, sizeof(buf), "B.W 0x%X", addr + 4 + off);
		return buf;
	}

	// Bcc.W
	if ((code1 & 0xF800) == 0xF000 && (code2 & 0xD000) == 0x8000) {
		uint32_t s = (code1 >> 10) & 1;
		uint32_t cond = (code1 >> 6) & 0xf;
		uint32_t j1 = (code2 >> 13) & 1;
		uint32_t j2 = (code2 >> 11) & 1;
		uint32_t i1 = ~(j1 ^ s) & 1;
		uint32_t i2 = ~(j2 ^ s) & 1;
		uint32_t imm6 = code1 & 0x3f;
		uint32_t imm11 = code2 & 0x7ff;
		int32_t off = (s << 20) | (i1 << 19) | (i2 << 18) | (imm6 << 12) | (imm11 << 1);
		if (s) off |= 0xFFE00000; // sign extend 21 bits
		const char* cc[] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","AL","NV"};
		snprintf(buf, sizeof(buf), "B%s.W 0x%X", cc[cond], addr + 4 + off);
		return buf;
	}

	// MOVW
	if ((code1 & 0xFBF0) == 0xF240) {
		uint32_t i = (code1 >> 10) & 1;
		uint32_t imm4 = code1 & 0xf;
		uint32_t imm3 = (code2 >> 12) & 0x7;
		uint32_t imm8 = code2 & 0xff;
		uint32_t rd = (code2 >> 8) & 0xf;
		uint32_t imm16 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;
		snprintf(buf, sizeof(buf), "MOVW %s, #0x%X", get_reg_name(rd).c_str(), imm16);
		return buf;
	}

	// MOVT
	if ((code1 & 0xFBF0) == 0xF2C0) {
		uint32_t i = (code1 >> 10) & 1;
		uint32_t imm4 = code1 & 0xf;
		uint32_t imm3 = (code2 >> 12) & 0x7;
		uint32_t imm8 = code2 & 0xff;
		uint32_t rd = (code2 >> 8) & 0xf;
		uint32_t imm16 = (imm4 << 12) | (i << 11) | (imm3 << 8) | imm8;
		snprintf(buf, sizeof(buf), "MOVT %s, #0x%X", get_reg_name(rd).c_str(), imm16);
		return buf;
	}

	snprintf(buf, sizeof(buf), "DCD 0x%04X%04X", code1, code2);
	return buf;
}

void dumpThumb2(const uint8_t* values, int base32, size_t size) {
	return;
	size_t i = 0;
	char line[256];
	while (i + 1 < size) {
		uint16_t code = values[i] | (values[i + 1] << 8);
		uint32_t addr = base32 + i;

		if ((code & 0xF800) >= 0xE800) {
				if (i + 3 < size) {
						uint16_t code2 = values[i + 2] | (values[i + 3] << 8);
						std::string dis = disasm_32bit(code, code2, addr);
						snprintf(line, sizeof(line), "%08X: %04X %04X  %s", addr, code, code2, dis.c_str());
						i += 4;
				} else {
						snprintf(line, sizeof(line), "%08X: %04X      DCW 0x%04X", addr, code, code);
						i += 2;
				}
		} else {
				std::string dis = disasm_thumb2_16bit(code, addr);
				snprintf(line, sizeof(line), "%08X: %04X      %s", addr, code, dis.c_str());
				i += 2;
		}
		std::cout << line << std::endl;
	}
	}
