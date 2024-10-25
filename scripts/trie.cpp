/*
    测试 trie 是使用 7bit + 1单元 声明数据还是 8bit 声明数据。
    本质是检查点的数量多还是数据的数量多。
    需要使用题目给出数据。所以读入也要做
*/

#include <bits/stdc++.h>

#define CHARSET 63
#define MAXS 1000
char getch( char c ) {
    if( c >= 'a' && c <= 'z' ) return c - 'a';
    if( c >= 'A' && c <= 'Z' ) return c - 'A' + 26;
    if( c >= '0' && c <= '9' ) return c + 52;
    if( c == '_' ) return 62;
    return -1;
}

struct trie {
    int t[]
};

int main() {
    return 0;
}
