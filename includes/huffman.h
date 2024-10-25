#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <utility>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <bitset>
#include "utils.h"

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
    std::string a;
    // long long maxVal, minVal;
    bool ifSign; // 是否有符号
    int length; // 长度
    bool inHuffman; // 是否在 value 哈夫曼树 TODO：如何判断是否应该加入哈夫曼树？
    // int dim; // 数组维数，0 表示非数组 TODO：查看下划线最后是否有数字、判断是否是数组
public:
    JSON_Key(): ifSign(false), length(0), inHuffman(false) {
        a.clear();
    }
    JSON_Key(const std::string &a, int length, bool ifSign, bool inHuffman):
        a(a), ifSign(ifSign), length(length), inHuffman(inHuffman) {}
    void outputStream() { // 输出自身。对于一个 JSON_String，需要输出 7bit 的字符串，以 0 结尾
       for(auto ch: a) outFile.append(7, ch);
       outFile.append(7, 0); // 分隔符
       outFile.append(6, length - 1);
    //    outFile.append(6, std::max(maxVal >= 0 ? ceilLog2(maxVal) : 0, minVal <= 0 ? ceilLog2(minVal) : 0) - 1); // 字符长度 - 1
       outFile.append(1, ifSign); // 是否有符号
       outFile.append(1, inHuffman); // 是否放入哈夫曼树
    }
    bool inHuff() {
        return inHuffman;
    }
    int getLen() {
        return length;
    }
    struct JSON_Key_Hasher {
        size_t operator()(const JSON_Key &key) const {
            return std::hash<std::string>{}(key.a);
        }
    };
    struct JSON_Key_Equals {
        bool operator()(const JSON_Key &a, const JSON_Key &b) const {
            return a.a == b.a;
        }
    };
};


class JSON_Value { // 键值的 value。
private: // 直接存储二进制值
    unsigned long long val;
public:
    JSON_Value(): val(0) {}
    JSON_Value(unsigned long long v): val(v) {}
    void outputStream() {
        if( val < 1 << 8 ) {
            outFile.append(2, 0);
            outFile.append(8, val);
        } else if( val < 1 << 16 ) {
            outFile.append(2, 1);
            outFile.append(16, val);
            // outFile.append(8, val >> 8);
            // outFile.append(8, val & (0b11111111));
        }
        else if( val < 1ll << 32 ) {
            outFile.append(2, 2);
            outFile.append(32, val);
            // for(int i = 3; i >= 0; i--)
            //     outFile.append(8, (val >> (8 * i)) & (0b11111111));
        }
        else {
            outFile.append(2, 3);
            outFile.append(64, val);
            // for(int i = 7; i >= 0; i--)
            //     outFile.append(8, (val >> (8 * i)) & (0b11111111));
        }
    }
    struct JSON_Value_Hasher {
        size_t operator() (const JSON_Value &value) const {
            return std::hash<unsigned long long>{}(value.val);
        }
    };
    struct JSON_Value_Equals {
        bool operator() (const JSON_Value &a, const JSON_Value &b) const {
            return a.val == b.val;
        }
    };
};

template<typename Key, typename KeyHasher, typename KeyEquals> // 压缩 key 的类型
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
    std::unordered_map<Key, int, KeyHasher, KeyEquals> hashTable; // 将 Key 映射到节点
    std::vector<Key> lookupTable; // 将节点映射到 Key
    std::vector<std::bitset<128> > encodeTable;
    std::vector<int> encodeTableLength;
    std::bitset<128> now;
    int key_tot;
    int key_cnt[MAXS+5];
    int getRightSkew( Node *p ) {
        if( p == nullptr ) return 0;
        int leftHeight = getRightSkew(p->lson);
        int rightHeight = getRightSkew(p->rson);
        if( leftHeight > rightHeight ) {
            std::swap(p->lson, p->rson);
            return leftHeight + 1;
        }
        return rightHeight + 1;
    }
    void huffmanEncode( Node *p ) {
        if( p->lson == nullptr ) { // 叶子节点，输出字符串信息
            outFile.append(1, 0); // 输出叶子节点
            // putchar('a');
            lookupTable[p->idx].outputStream();
        } else {
            outFile.append(1, 1); // 输出非叶子节点
            // putchar('b');
            huffmanEncode(p->lson);
            huffmanEncode(p->rson);
        }
    }
    void encodeDfs(Node *p, int depth = 0) {
        if( p->lson == nullptr ) { // 叶子节点
            encodeTable[p->idx] = now;
            encodeTableLength[p->idx] = depth;
        } else { //
            now[depth] = 0;
            encodeDfs(p->lson, depth+1);
            now[depth] = 1;
            encodeDfs(p->rson, depth+1);
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
    int getEncodeTableLength(const int idx) {
        return encodeTableLength[idx];
    }
    int getEncodeTableLength(const Key &key) {
        return encodeTableLength[hashTable[key]];
    }
    std::bitset<128> getEncodeTable(const int idx) {
        return encodeTable[idx];
    }
    std::bitset<128> getEncodeTable(const Key &key) {
        return encodeTable[hashTable[key]];
    }
    Key getLookupTable(const int idx) {
        return lookupTable[idx];
    }
    void reset() {
        for(int i = 1; i <= 2 * key_tot - 1; ++i) if( forest[i] != nullptr )
            delete forest[i];
        for(int i = 1; i <= key_tot; ++i) key_cnt[i] = 0;
        hashTable.clear();
        lookupTable.clear();
        lookupTable.push_back(Key());
        key_tot = 0;
    }
    void insert( const Key &key, int weight ) {
        int key_id;
        if( hashTable.count(key) == 0 ) {
            hashTable[key] = ++key_tot;
            key_id = key_tot;
            lookupTable.push_back(key);
        } else key_id = hashTable[key];
        key_cnt[key_id] += weight;
    }
    void createHuffmanTree() {
        // 对于 1 ~ key_tot 的标号，需要知道其在最终树上的位置
        if( key_tot == 0 ) return;
        static std::pair<int, int> a[MAXS+5];
        for(int i = 1; i <= key_tot; ++i) {
            forest[i] = new Node(key_cnt[i], i);
            a[i] = std::make_pair(key_cnt[i], i);
        }
        sort(a+1, a+1+key_tot);
        int idxa = 1, idxb = key_tot + 1, totb = key_tot;
        for(int i = 1; i < key_tot; ++i) {
            int w = 0;
            int min1Arg , min2Arg;
            if( idxb <= totb && a[idxa] > a[idxb] || idxa > key_tot ) {
                w += a[idxb].first;
                min1Arg = a[idxb].second;
                ++idxb;
            } else {
                w += a[idxa].first;
                min1Arg = a[idxa].second;
                ++idxa;
            }
            if( idxb <= totb && a[idxa] > a[idxb] || idxa > key_tot ) {
                w += a[idxb].first;
                min2Arg = a[idxb].second;
                ++idxb;
            } else {
                w += a[idxa].first;
                min2Arg = a[idxa].second;
                ++idxa;
            }
            ++totb;
            a[totb] = std::make_pair(w, totb);
            forest[totb] = new Node(w, totb, forest[min1Arg], forest[min2Arg]);
        }
        getRightSkew(forest[totb]);
        encodeTable.resize(key_tot+1);
        encodeTableLength.resize(key_tot+1);
        encodeDfs(forest[totb]);
        // printf("%d %d\n", totb, key_tot);
    }
    void outputHuffman() {
        // 此处作用是将 huffman 树存到比特流里，实际使用时还是要在原哈夫曼树上跑。
        // 可以直接判断哈夫曼树的结尾，所以不需要分隔符。
        // printf("%d\n", 2 * key_tot - 1);
        outFile.append(1, key_tot != 0);
        if( key_tot == 0 ) return;
        huffmanEncode(forest[2*key_tot-1]);      
    }
};

Huffman<JSON_Key, JSON_Key::JSON_Key_Hasher, JSON_Key::JSON_Key_Equals> keyHuff;
Huffman<JSON_Value, JSON_Value::JSON_Value_Hasher, JSON_Value::JSON_Value_Equals> valueHuff;

void compressOutputSpecialChar(const std::vector<std::string> &compress_string) {
    for(auto v: compress_string) {
        int len = v.size();
        for(int i = 0; i < len; ++i) {
            outFile.append(7, v[i]);
        }
        outFile.append(7, 0); // 分隔符
    }
    outFile.append(7, 0);
}

void compressOutputKeyHuffman(const std::vector<std::string> &string_list, const int Is_array[], const int Count_times[], const int length[], const bool ifSign[], const bool inHuffman[] , const int file_num ) {
    for(int i = 0; i < string_list.size(); ++i) {
        // printf("Test data:");
        // std::cout << string_list[i] << ' ';
        // printf("%d\n", ifSign[i]);
        keyHuff.insert(JSON_Key(string_list[i], length[i], ifSign[i], inHuffman[i]), Count_times[i]);
    }
    keyHuff.insert(JSON_Key("", 0, 0, 0), file_num); // 插入结束字符

    keyHuff.createHuffmanTree();
    keyHuff.outputHuffman();
}

void compressOutputValueHuffman(const std::unordered_map<long long, int> set[], int n, const bool inHuffman[]){
    for(int i = 0; i < n; ++i) {
        if( inHuffman[i] ) {
            for(auto iter: set[i]) {
				auto key = iter.first;
				auto value = iter.second;
                valueHuff.insert(JSON_Value(key), value);
            }
        }
    }
    valueHuff.createHuffmanTree();
    // puts("F");
    valueHuff.outputHuffman();
}

void compressOutputJSON(struct JSON_my json[], int file_num) {
    int divLength = keyHuff.getEncodeTableLength(JSON_Key("", 0, 0, 0));
    std::bitset<128> divStream = keyHuff.getEncodeTable(JSON_Key("", 0, 0, 0));
    for(int i = 0; i < file_num; i++) {
        // outFile.outputLen();
        auto vect = json[i].vec;
        sort(vect.begin(), vect.end());
        for(int j = 0; j < vect.size(); ++j) {
            // 输出 key
            outFile.append(keyHuff.getEncodeTableLength(vect[j].key+1), keyHuff.getEncodeTable(vect[j].key+1));
            // 输出 value
            bool huffFlag = keyHuff.getLookupTable(vect[j].key+1).inHuff();
            int valueLeng = keyHuff.getLookupTable(vect[j].key+1).getLen();
            if( vect[j].dimension[0] == 0 ) { // 如果是数组，则采用 length-value 的存储方式
                int p = j;
                while( p < vect.size() - 1 && vect[p+1].key == vect[p].key ) // 到数组的末尾
                    ++p;
                outFile.append(3, vect[p].dimension[0]); // 输出第一维数组的长度
                if( vect[j].dimension[1] == 0 ) { // 二维数组
                    for(int k = j; k <= p; ++k) {
                        int l = k;
                        while( l < vect.size() - 1 && vect[l+1].dimension[0] == vect[l].dimension[0] && vect[l+1].key == vect[l].key ) 
                            ++l;
                        outFile.append(3, vect[l].dimension[1]);
                        for(int o = k; o <= l; ++o) {
                            if( huffFlag ) {
                                outFile.append(valueHuff.getEncodeTableLength(JSON_Value(vect[o].value)), valueHuff.getEncodeTable(JSON_Value(vect[o].value)));
                            } else {
                                outFile.append(valueLeng, vect[o].value);
                            }    
                        }
                        k = l;
                    }
                } else { // 一维数组
                    for(int k = j; k <= p; ++k) {
                        if( huffFlag ) {
                            outFile.append(valueHuff.getEncodeTableLength(JSON_Value(vect[k].value)), valueHuff.getEncodeTable(JSON_Value(vect[k].value)));
                        } else {
                            outFile.append(valueLeng, vect[k].value);
                        }    
                    }
                }
                j = p; // 数组一次性存完，跳过
            } else { // 否则直接存 value （还需要判断是否存入 value huffman）
                if( huffFlag ) {
                    outFile.append(valueHuff.getEncodeTableLength(JSON_Value(vect[j].value)), valueHuff.getEncodeTable(JSON_Value(vect[j].value)));
                } else {
                    // printf("value Len:%d\n", valueLeng);
                    // printf("len: %d, value: %d\n", valueLeng, vect[j].value);
                    long long val = vect[j].value;
                    if( val < 0 ) {
                        val = -val;
                        val ^= (1<<valueLeng) - 1;
                        val += 1;
                    }
                    outFile.append(valueLeng, vect[j].value);
                }
            }
        }
        outFile.append(divLength, divStream);
    }
    outFile.append(divLength, divStream); // 文件结束
}

#endif
