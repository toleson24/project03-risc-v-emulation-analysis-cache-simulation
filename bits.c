#include "bits.h"

uint32_t get_bits(uint64_t num, uint32_t start, uint32_t count) {
	uint32_t num_shifted = num >> start;
	uint32_t mask = (1 << count) - 1;
	uint32_t bits = num_shifted & mask;
	
	return bits;
}

int64_t sign_extend(uint64_t num, uint32_t start) {
	num = num << (64 - start);
	int64_t sign_ext_num = ((int64_t) num >> (64 - start));
	
	return sign_ext_num;
}

bool get_bit(uint64_t num, uint32_t which) {
	uint32_t num_shifted = num >> which;
	uint32_t mask = (1 << 1) - 1;
	return num_shifted & mask;
}

