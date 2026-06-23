
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cctype>
#include <cstdint>

void dumpMem(const uint8_t* raw, int base32, size_t size) {
	std::stringstream ss;
	for (size_t i = 0; i < size; i += 16) {
		ss << std::hex << std::setfill('0') << std::setw(8) << (base32+i) << "  ";
		for (size_t j = 0; j < 16; ++j) {
			if (i + j < size) {
				ss << std::setw(2) << static_cast<int>(raw[i + j]) << " ";
			} else {
				ss << "   ";
			}
			if (j == 7) {
				ss << " ";
			}
		}
		ss << " |";
		for (size_t j = 0; j < 16; ++j) {
			if (i + j < size) {
				char c = static_cast<char>(raw[i + j]);
				ss << (std::isprint(static_cast<unsigned char>(c)) ? c : '.');
			} else {
				ss << " ";
			}
		}
		ss << "|\n";
	}
	std::cout << ss.str();
}
