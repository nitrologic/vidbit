// elfutil.cpp

#include "elf32.h"
#include <iostream>
#include <fstream>
#include <cstdint>
#include <cstring>

void dumpMem(const uint8_t* values, int base32, size_t size);

typedef std::vector<char> bytes;

bytes load_elf32(const char* path, struct elf32& hdr){
	bytes result;
	std::ifstream file(path, std::ios::binary|std::ios::ate);
	if (!file) return result;
	std::streamsize size = file.tellg();
	file.seekg(0);
	file.read(reinterpret_cast<char*>(&hdr), sizeof(hdr));
	if (file.gcount() != sizeof(hdr)) return result;
// Validate ELF magic
	if (hdr.e_ident[0] != 0x7f || hdr.e_ident[1] != 'E' || hdr.e_ident[2] != 'L' || hdr.e_ident[3] != 'F') return result;
	std::streamsize count=size-sizeof(hdr);
	result.resize(count);
	file.read(reinterpret_cast<char*>(result.data()), count);
	return result;
}

std::string emachine_string(uint16_t machine){
	auto it = emachines.find(machine);
	if (it != emachines.end()) return it->second;
	return "UNKNOWN";
}

void print_elf32_header(const struct elf32& hdr)
{
	std::string emachine=emachine_string(hdr.e_machine);
	std::cout << "[ELF32] header:{" << std::hex
		<< "e_type:0x" << hdr.e_type << "[" << etypes[hdr.e_type] << "], "
		<< "e_machine:0x" << hdr.e_machine << "[" << emachine << "], "
		<< "e_entry:0x" << hdr.e_entry << ", "
		<< "e_phoff:0x" << hdr.e_phoff << ", "
		<< "e_shoff:0x" << hdr.e_shoff << ", "
		<< std::dec
		<< "e_phnum:" << hdr.e_phnum << ", "
		<< "e_shnum:" << hdr.e_shnum << ", "
		<< "e_shstrndx:" << hdr.e_shstrndx << "}" << std::endl;
}

const char* get_shstrtab(const bytes& content, const struct elf32& hdr, const struct elf32_shdr* shdrs){
	if (hdr.e_shstrndx >= hdr.e_shnum) return nullptr;
	const elf32_shdr& strsec = shdrs[hdr.e_shstrndx];
	return content.data() + (strsec.sh_offset - sizeof(elf32));
}

const char* get_entry_symbol(const bytes& content, const struct elf32& hdr, const struct elf32_shdr* shdrs) {
	for (int i = 0; i < hdr.e_shnum; i++) {
		if (shdrs[i].sh_type == 2) { // 2 = SHT_SYMTAB
			// sh_link contains the index of the associated string table
			uint32_t strtab_idx = shdrs[i].sh_link;
			const char* sym_strtab = content.data() + (shdrs[strtab_idx].sh_offset - sizeof(elf32));
			elf32_sym* syms = (elf32_sym*)(content.data() + (shdrs[i].sh_offset - sizeof(elf32)));
			int sym_count = shdrs[i].sh_size / sizeof(elf32_sym);
			for (int j = 0; j < sym_count; j++) {
				if (syms[j].st_value == hdr.e_entry) {
					return sym_strtab + syms[j].st_name;
				}
			}
		}
	}
	return nullptr;
}


void print_symbols(const bytes& content, const struct elf32& hdr, const struct elf32_shdr* shdrs) {
	for (int i = 0; i < hdr.e_shnum; i++) {
		if (shdrs[i].sh_type == 2) { // 2 = SHT_SYMTAB
			uint32_t strtab_idx = shdrs[i].sh_link;
			const char* sym_strtab = content.data() + (shdrs[strtab_idx].sh_offset - sizeof(elf32));
			elf32_sym* syms = (elf32_sym*)(content.data() + (shdrs[i].sh_offset - sizeof(elf32)));
			int sym_count = shdrs[i].sh_size / sizeof(elf32_sym);

			std::cout << "[ELF32] symbols (" << sym_count << "):" << std::endl;
			for (int j = 0; j < sym_count; j++) {
					const char* name = sym_strtab + syms[j].st_name;
					// Some symbols have no name (st_name == 0), handle them gracefully
					if (syms[j].st_name == 0) name = "<null>";

					std::cout << "[ELF32] sym" << j << ":{name:\"" << name
								<< "\", value:0x" << std::hex << syms[j].st_value
								<< ", size:0x" << syms[j].st_size
								<< "}" << std::dec << std::endl;
			}
		}
	}
}

int main(int argc, char* argv[]){
	std::cout << "elfutil 0.2" << std::endl;
	if (argc < 2){
		std::cerr << "usage: elfutil <elf32-file>" << std::endl;
		return 1;
	}
	struct elf32 elf32header;
	const char *path=argv[1];

	bytes content=load_elf32(path, elf32header);
	if (content.size()==0){		
		std::cerr << "load_elf32_header failure for path:" << path << std::endl;
		return 1;
	}
	std::cout << "[ELF32] path:" << path << std::endl;

	print_elf32_header(elf32header);

	elf32_phdr *ph=(elf32_phdr*)(content.data()+(elf32header.e_phoff-sizeof(elf32)));
	elf32_shdr *sh=(elf32_shdr*)(content.data()+(elf32header.e_shoff-sizeof(elf32)));

//	print_symbols(content, elf32header, sh);

	const char* strtab = get_shstrtab(content, elf32header, sh);

	for(int i=0;i<elf32header.e_phnum;i++){
		auto ptype=ptypes.find(ph[i].p_type)->second;
		std::cout << "[ELF32] p" << i << ":{" << std::hex
			<< "type:0x" << (ph[i].p_type) << "[" << ptype << "]"
			<< ", vaddr:0x" << ph[i].p_vaddr 
			<< ", memsz:0x" << ph[i].p_memsz << ", filesz:0x" << ph[i].p_filesz
			<< ", flags:" << pflags_string(ph[i].p_flags)
			<< "}" << std::endl << std::dec;
	}

	for(int i=0;i<elf32header.e_shnum;i++){
		auto shtype=shtypes.find(sh[i].sh_type)->second;
		const char* name = strtab + sh[i].sh_name;
		std::cout << "[ELF32] sh" << i << ":{name:\"" << name << "\", type:" << shtype <<  "}" << std::endl;
//		std::cout << "sh" << i << " type:"<<sh[i].sh_type<<" size:"<<sh[i].sh_size << " entsize:" << sh[i].sh_entsize << std::endl;
		if (strcmp(name, ".text") == 0) {
	        dumpMem(reinterpret_cast<const uint8_t*>(content.data() + (sh[i].sh_offset - sizeof(elf32))), sh[i].sh_offset, sh[i].sh_size);
		}
	}

}