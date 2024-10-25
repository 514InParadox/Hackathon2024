#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <string.h>
#include <bitset>
#include <vector>

class Bitstream {
private:
	// 64 * 1024 * 8 bit
	static constexpr int MAXLEN = 1 << 6 << 10 << 3;
    bool buf[MAXLEN+16];
    int len;
	bool *p;
public:
    Bitstream(): len(0) {
    }
    void append( int l, size_t num ) { // 一次不会增加 64 位以上，增加的数直接用 size_t 存储
        for(int i = 0; i < l; ++i) 
            buf[len + i] = (num >> i) & 1;
        len += l;
    }
    void append( int l, std::bitset<128> num ) { // 重载哈夫曼编码 append
        for(int i = 0; i < l; ++i) buf[len + i] = num[i];
        len += l;
    }
    void flushInto(const std::string &path) {
        int bytes = (len + 7) / 8;
        std::ofstream outfile(path, std::ios::binary);
        if( !outfile ) {
            throw std::ios_base::failure("Failed to open file for writing.");
        }

        for(int i = 0; i < bytes; i ++) {
            unsigned char byte = 0;
            for(int j = 0; j < 8 && i * 8 + j < len; ++j)
                byte |= (buf[i*8 + j] << j);
            outfile.write(reinterpret_cast<char *>(&byte), 1);
        }

        outfile.close();
        len = 0;
    }

	void syncFrom(const std::string &path) {
		std::ifstream fs(path, std::ios::binary);
		constexpr std::size_t MAXLEN_ = 1 << 6 << 10;
		static char buf_[MAXLEN_];
		fs.read(buf_, MAXLEN_);
		len = 0;
		for (std::size_t len_ = fs.gcount(), i = 0; i < len_; ++i)
			for (int j = 0; j < 8; ++j)
				buf[len++] = buf_[i] >> j & 1;
		p = buf;
	}
	std::uint64_t extract(int w) {
		std::uint64_t res = 0;
		for (int i = 0; i < w; ++i)
			res |= static_cast<std::uint64_t>(*p++) << i;
		return res;
	}
    void outputLen() {
        printf("outFile len: %d\n", len);
    }
};

extern Bitstream outFile;

char enc_to_ch(std::int8_t e);
std::int8_t ch_to_enc(char c);

struct JSON_my{
    struct data{
        int key,dimension[2];
        long long value;
        bool operator <(const data &other) {
            return this->key < other.key ||
            this->key == other.key && this->dimension[0] < other.dimension[0] ||
            this->key == other.key && this->dimension[0] == other.dimension[0] && this->dimension[1] < other.dimension[1];

        }
    };
    std::vector<data> vec;
};

// int main() {
//     Bitstream test;
//     test.append(8, 'a');
//     test.append(8, 'b');
//     test.flushInto("./test");
// 	test.syncFrom("./test");
// 	assert(test.extract(8) == 'a');
// 	assert(test.extract(8) == 'b');
// 	auto check = [](std::int8_t e, char c) {
// 		assert(enc_to_ch(e) == c);
// 		assert(ch_to_enc(c) == e);
// 	};
// 	check(1, 'A');
// 	check(26, 'Z');
// 	check(27, 'a');
// 	check(52, 'z');
// 	check(53, '0');
// 	check(62, '9');
// 	check(63, '_');
// }

#endif
