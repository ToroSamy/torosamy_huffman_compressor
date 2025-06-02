#ifndef HUFFMAN_DECOMPRESSOR_H
#define HUFFMAN_DECOMPRESSOR_H

#include <string>
#include <fstream>
#include <map>
#include "Huffman.h"

class HuffmanDecompressor {
public:
    HuffmanDecompressor();
    void decompress(const std::string& input_filepath, const std::string& output_filepath);
private:
    HuffmanNode header_[512]; // 用于解压时存储哈夫曼表


    void readFileHeader(std::ifstream& ifp, long& original_file_length, long& huffman_table_start_pos);
    long readHuffmanTable(std::ifstream& ifp, long huffman_table_start_pos);
    void sortHuffmanCodesByLength(long num_chars_in_table);
    long decodeAndWriteData(std::ifstream& ifp, std::ofstream& ofp, long original_file_length, long num_chars_in_table);


};

#endif // HUFFMAN_DECOMPRESSOR_H