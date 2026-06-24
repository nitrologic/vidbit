// elf32.h

#pragma once

#include <cstdint>
#include <vector>
#include <map>
#include <string>

void dumpMem(const uint8_t* values, int base32, size_t size);


enum etype{ET_NONE,ET_REL,ET_EXEC,ET_DYN,ET_CORE};
enum emachine{EM_NONE = 0, EM_386 = 3, EM_MIPS = 8, EM_ARM = 0x28, EM_X86_64 = 0x3e};

const std::vector<std::string> etypes = {"ET_NONE", "ET_REL", "ET_EXEC", "ET_DYN", "ET_CORE"};
const std::map<uint16_t, std::string> emachines = {{0,"EM_NONE"},{3,"EM_386"},{8,"EM_MIPS"},{0x28,"EM_ARM"},{0x3e,"EM_X86_64"}};

const std::map<uint32_t, std::string> ptypes = {
	{0, "NULL"}, {1, "LOAD"}, {2, "DYNAMIC"}, {3, "INTERP"}, {4, "NOTE"}, {6, "PHDR"}, {7, "TLS"}, 
	{0x6474e551, "GNU_EH_FRAME"}, 
	{0x70000001, "ARM_EXIDX"}
};

std::string typeString(uint32_t ptype){
	auto it = ptypes.find(ptype);
	if (it != ptypes.end()) return it->second;
	return "[PTYPE_UNKNOWN]";
}

const std::map<uint32_t, std::string> shtypes = {
	{0, "NULL"}, {1, "PROGBITS"}, {2, "SYMTAB"}, {3, "STRTAB"}, {4, "RELA"}, {5, "HASH"}, {6, "DYNAMIC"}, {7, "NOTE"}, {8, "NOBITS"}, {9, "REL"}, {11, "DYNSYM"},
	{0x70000003, "ARM_ATTRIBUTES"},
	{0x70000001, "ARM_EXIDX"}
};

std::string pflags_string(uint32_t flags) {
	std::string s;
	s += (flags & 0x4) ? 'R' : '-';
	s += (flags & 0x2) ? 'W' : '-';
	s += (flags & 0x1) ? 'X' : '-';
	return s;
}
struct elf32_phdr {
	uint32_t p_type;
	uint32_t p_offset;
	uint32_t p_vaddr;
	uint32_t p_paddr;
	uint32_t p_filesz;
	uint32_t p_memsz;
	uint32_t p_flags;
	uint32_t p_align;
};

struct elf32_shdr {
	uint32_t sh_name;
	uint32_t sh_type;
	uint32_t sh_flags;
	uint32_t sh_addr;
	uint32_t sh_offset;
	uint32_t sh_size;
	uint32_t sh_link;
	uint32_t sh_info;
	uint32_t sh_addralign;
	uint32_t sh_entsize;
};

struct elf32{
    uint8_t  e_ident[16];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uint32_t e_entry;
    uint32_t e_phoff;
    uint32_t e_shoff;
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;
    uint16_t e_phnum;
    uint16_t e_shentsize;
    uint16_t e_shnum;
    uint16_t e_shstrndx;
};

struct elf32_sym {
	uint32_t st_name;
	uint32_t st_value;
	uint32_t st_size;
	uint8_t  st_info;
	uint8_t  st_other;
	uint16_t st_shndx;
};