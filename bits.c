#include "bits.h"

uint32_t get_bits(uint64_t num, uint32_t start, uint32_t count) {
	uint32_t num_shifted = num >> start;
	uint32_t mask = (1 << count) - 1;
	uint32_t bits = num_shifted & mask;
	
	return bits;
}

int64_t sign_extend(uint64_t num, uint32_t start) {
	/*int64_t signed_bit = (num >> (start - 1)) & 1;
	int64_t sign_extended_num = signed_bit ? ~0 << start : 0; 
		
	return (int64_t) num | sign_extended_num;*/
	
	num = num << (64 - start);
	int64_t x = ((int64_t) num >> (64 - start));
	
	return x;
}

bool get_bit(uint64_t num, uint32_t which) {
	uint32_t num_shifted = num >> which;
	uint32_t mask = (1 << 1) - 1;
	return num_shifted & mask;
}

