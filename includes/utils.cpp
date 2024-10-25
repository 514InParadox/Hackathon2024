#ifndef UTILS_H
#define UTILS_H

#include <string.h>
#include <string>
#include <fstream>

#define MAXLEN 65536
// 64 * 1024
class Bitstream {
private:
    bool buf[MAXLEN+16];
    int len;
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
            outfile.put(byte);
        }

        outfile.close();
    }
};

int main() {
    Bitstream test;
    test.append(8, 'a');
    test.append(8, 'b');
    test.flushInto("./test");
}

#endif