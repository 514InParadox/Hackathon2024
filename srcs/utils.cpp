#include "../includes/utils.h"

char enc_to_ch(std::int8_t e) {
	return e <= 26 ? 64 | e : e <= 52 ? 96 | e - 26 : e <= 62 ? 48 | e - 53 : '_';
}

std::int8_t ch_to_enc(char c) {
	return std::isupper(c) ? c & 31 : std::islower(c) ? 26 + (c & 31) : std::isdigit(c) ? 53 + (c & 15) : 63;
}

Bitstream outFile;
