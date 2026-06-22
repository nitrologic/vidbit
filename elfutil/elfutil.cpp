#include "elf32.h"
#include <iostream>
#include <iostream>
#include <fstream>
#include <cstring>

bool load_elf32_header(const char* path, struct elf32& hdr){
	std::ifstream file(path, std::ios::binary);
	if (!file) return false;
	file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	if (file.gcount() != sizeof(hdr)) return false;
// Validate ELF magic
	if (hdr.e_ident[0] != 0x7f || hdr.e_ident[1] != 'E'  || hdr.e_ident[2] != 'L'  || hdr.e_ident[3] != 'F') return false;
	return true;
}

void print_elf32_header(const struct elf32& hdr)
{
	std::cout << "e_type:      0x" << std::hex << hdr.e_type      << std::endl;
	std::cout << "e_machine:   0x" << std::hex << hdr.e_machine   << std::endl;
	std::cout << "e_entry:     0x" << std::hex << hdr.e_entry     << std::endl;
	std::cout << "e_phoff:     0x" << std::hex << hdr.e_phoff     << std::endl;
	std::cout << "e_shoff:     0x" << std::hex << hdr.e_shoff     << std::endl;
	std::cout << "e_phnum:     "   << std::dec << hdr.e_phnum     << std::endl;
	std::cout << "e_shnum:     "   << std::dec << hdr.e_shnum     << std::endl;
	std::cout << "e_shstrndx:  "   << std::dec << hdr.e_shstrndx  << std::endl;
}

int main(int argc, char* argv[]){
	std::cout << "elfutil 0.1" << std::endl;
	if (argc < 2){
		std::cerr << "usage: elfutil <elf32-file>" << std::endl;
		return 1;
	}
	struct elf32 elf32header;
	if (!load_elf32_header(argv[1], elf32header)){
		std::cerr << "error: failed to read ELF32 header from '" << argv[1] << "'" << std::endl;
		return 1;
	}

	print_elf32_header(elf32header);

}