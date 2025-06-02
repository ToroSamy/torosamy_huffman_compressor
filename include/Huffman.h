#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <string>
#include <climits> 
class HuffmanNode {
public:
    unsigned char b;     // 字符本身
    long count;          // 字符出现频率（权值）/ 或在解压时表示编码长度
    long parent;         // 父节点索引
    long lch, rch;       // 左右子节点索引
    std::string bits;    // 存储哈夫曼编码

    HuffmanNode() : b(0), count(0), parent(-1), lch(-1), rch(-1), bits("") {}

    bool operator<(const HuffmanNode& other) const {
        return count > other.count; // 降序
    }
};

// 将字节转换为二进制字符串，固定8位
std::string byteToBinaryString(unsigned char byte);

#endif // HUFFMAN_H