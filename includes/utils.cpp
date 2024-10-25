#ifndef UTILS_H
#define UTILS_H

#include <cassert>
#include <cstddef>
#include <fstream>
#include <string>
#include <string.h>

constexpr int MAXLEN = 1 << 6 << 10 << 3;
// 64 * 1024 * 8 bit
class Bitstream {
private:
    bool buf[MAXLEN+16];
    int len;
	bool *p;
public:
    Bitstream(): len(0) {
        memset(buf, 0, sizeof(buf));
    }
    void append( int l, size_t num ) { // 一次不会增加 64 位以上，增加的数直接用 size_t 存储
        for(int i = 0; i < l; ++i) 
            buf[len + i] = (num >> i) & 1;
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
	std::size_t extract(int w) {
		std::size_t res = 0;
		for (int i = 0; i < w; ++i)
			res |= *p++ << i;
		assert(p <= buf + len);
		return res;
	}
};

int main() {
    Bitstream test;
    test.append(8, 'a');
    test.append(8, 'b');
    test.flushInto("./test");
	test.syncFrom("./test");
	assert(test.extract(8) == 'a');
	assert(test.extract(8) == 'b');
}

#endif
