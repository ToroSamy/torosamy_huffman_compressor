#ifndef HUFFMAN_COMPRESSOR_H
#define HUFFMAN_COMPRESSOR_H

#include <string>
#include <fstream>
#include "Huffman.h"

class HuffmanCompressor {
public:
    HuffmanCompressor();
    void compress(const std::string& input_filepath, const std::string& output_filepath);
private:
    HuffmanNode header_[512]; // 前256个元素存储叶子结点,其余存储非叶子结点
    long calculateFrequencies(std::ifstream& ifp);
    void sortNodesByFrequency(long num_distinct_chars);
    long buildHuffmanTree(long num_distinct_chars);
    void generateHuffmanCodes(long num_distinct_chars);
    long writeCompressedData(std::ifstream& ifp, std::ofstream& ofp, long original_file_length, long num_distinct_chars);
    void writeHuffmanTable(std::ofstream& ofp, long num_distinct_chars);


};

#endif // HUFFMAN_COMPRESSOR_H