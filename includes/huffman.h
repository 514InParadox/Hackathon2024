#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <utility>
#include <unordered_map>
#include <vector>
#include "utils.h"

template <typename T>
T max( T a , T b ) { return a > b ? a : b; }
int ceilLog2( long long x ) { // 判断需要表示的长度
    if( x == 0 || x == -1 ) return 1;
    int ret = 0;
    if( x > 0 ) {
        while( x ) ++ret, x >>= 1;
    } else {
        x = -(x + 1);
        while( x ) ++ret, x >>= 1;
    }
    return ret;
}

class JSON_Key { // 键值的字符串表示，注意这里是使用过特殊字符编码的。
private: // 直接存储编码后的字符串
    std::vector<char> a;
    long long maxVal, minVal;
    bool typed; // 是否有符号
    bool inHuffman; // 是否在 value 哈夫曼树 TODO：如何判断是否应该加入哈夫曼树？
    // int dim; // 数组维数，0 表示非数组 TODO：查看下划线最后是否有数字、判断是否是数组
public:
    JSON_Key(): maxVal(1ll<<63), minVal((1ull<<63)-1), typed(false), inHuffman(false) {
        a.clear();
    }
    void outputStream() { // 输出自身。对于一个 JSON_String，需要输出 7bit 的字符串，以 0 结尾
       for(auto ch: a) outFile.append(7, ch);
       outFile.append(7, 0); // 分隔符
       outFile.append(6, max(maxVal >= 0 ? ceilLog2(maxVal) : 0, minVal <= 0 ? ceilLog2(minVal) : 0) - 1); // 字符长度 - 1
       outFile.append(1, minVal < 0); // 是否有符号
       outFile.append(1, inHuffman); // 是否放入哈夫曼树
    }
};

class JSON_Value { // 键值的 value。
private: // 直接存储二进制值
    unsigned long long val;
public:
    JSON_Value(): val(0) {}
    void outputStream() {
        if( val < 1 << 8 ) {
            outFile.append(2, 0);
            outFile.append(8, val);
        } else if( val < 1 << 16 ) {
            outFile.append(2, 1);
            outFile.append(8, val >> 8);
            outFile.append(8, val & (0b11111111));
        }
        else if( val < 1ll << 32 ) {
            outFile.append(2, 2);
            for(int i = 3; i >= 0; i--)
                outFile.append(8, (val >> (8 * i)) & (0b11111111));
        }
        else {
            outFile.append(2, 3);
            for(int i = 7; i >= 0; i--)
                outFile.append(8, (val >> (8 * i)) & (0b11111111));
        }
    }
};

template<typename Key> // 压缩 key 的类型
class Huffman {
#define MAXS 5000
private:
    struct Node{
        int weight, idx;
        Node *lson, *rson;
        // Node(): weight(0), idx(0), lson(nullptr), rson(nullptr) {}
        Node(int w, int i): weight(w), idx(i), lson(nullptr), rson(nullptr) {}
        Node(int w, int i, Node *ls, Node *rs): weight(w), idx(i), lson(ls), rson(rs) {}
    };
    Node *forest[MAXS*2+5]; // 哈夫曼树的节点个数为 key_tot * 2 - 1
    std::unordered_map<Key, int> hashTable; // 将 Key 映射到节点
    std::vector<Key> lookupTable; // 将节点映射到 Key
    int key_tot;
    int key_cnt[MAXS+5];
    int getRightSkew( Node *p ) {
        if( p == nullptr ) return 0;
        int leftHeight = getRightSkew(p->lson);
        int rightHeight = getRightSkew(p->rson);
        if( leftHeight > rightHeight ) {
            swap(p->lson, p->rson);
            return leftHeight + 1;
        }
        return rightHeight + 1;
    }
    void huffmanEncode( Node *p ) {
        if( p->lson == nullptr ) { // 叶子节点，输出字符串信息
            outFile.append(1, 0); // 输出叶子节点
            lookupTable[p->idx].outputStream();
        } else {
            outFile.append(1, 1); // 输出非叶子节点
            huffmanEncode(p->lson);
            huffmanEncode(p->rson);
        }
    }
public:
    Huffman() {
        for(int i = 1; i <= MAXS * 2; ++i) forest[i] = nullptr;
        hashTable.clear();
        lookupTable.clear();
        lookupTable.push_back(Key()); // 数组下标从 1 开始
        key_tot = 0;
        memset(key_cnt, 0, sizeof(key_cnt));
    }
    void insert( const Key &key, long long value ) {
        int key_id;
        if( hashTable.count(key) == 0 ) {
            hashTable[key] = ++key_tot;
            key_id = key_tot;
            lookupTable.push_back(key);
        } else key_id = hashTable[key];
        ++key_cnt[key_id];
    }
    void createHuffmanTree() {
        // 对于 1 ~ key_tot 的标号，需要知道其在最终树上的位置
        static pair<int, int> a[MAXS+5], b[MAXS+5];
        for(int i = 1; i <= key_tot; ++i) {
            forest[i] = new Node(key_cnt[i], i);
            a[i] = make_pair(key_cnt[i], i);
        }
        sort(a+1, a+1+n);
        int idxa = 1, idxb = 1, totb = 0;
        for(int i = 1; i < n; ++i) {
            int w = 0;
            std::pair<int, int> min1Arg , min2Arg;
            if( idxb <= totb && a[idxa] > b[idxb] || idxa > key_tot ) {
                w += b[idxb].first;
                min1Arg = b[idxb].second;
                ++idxb;
            } else {
                w += a[idxa].first;
                min1Arg = a[idxa].second;
                ++idxa;
            }
            if( idxb <= totb && a[idxa] > b[idxb] || idxa > key_tot ) {
                w += b[idxb].first;
                min2Arg = b[idxb].second;
                ++idxb;
            } else {
                w += a[idxa].first;
                min2Arg = a[idxa].second;
                ++idxa;
            }
            ++totb;
            b[totb] = make_pair(w, totb);
            forest[totb] = Node(w, totb, forest[min1Arg], forest[min2Arg]);
        }
        getRightSkew(forest[totb]);
        
    }
    void outputHuffman() {
        // 此处作用是将 huffman 树存到比特流里，实际使用时还是要在原哈夫曼树上跑。
        // 可以直接判断哈夫曼树的结尾，所以不需要分隔符。
        huffmanEncode(forest[totb]);      
    }
};

int main() {
    // union test
    return 0;
}

#endif