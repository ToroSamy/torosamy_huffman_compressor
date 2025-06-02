#include "HuffmanDecompressor.h"
#include <iostream>
#include <algorithm>
#include <stdexcept> 
#include <filesystem>
HuffmanDecompressor::HuffmanDecompressor() {
}

void HuffmanDecompressor::readFileHeader(std::ifstream& ifp, long& original_file_length, long& huffman_table_start_pos) {
    ifp.read(reinterpret_cast<char*>(&original_file_length), sizeof(long));
    ifp.read(reinterpret_cast<char*>(&huffman_table_start_pos), sizeof(long));
}

long HuffmanDecompressor::readHuffmanTable(std::ifstream& ifp, long huffman_table_start_pos) {
    ifp.seekg(huffman_table_start_pos, std::ios::beg);
    long num_chars_in_table;
    ifp.read(reinterpret_cast<char*>(&num_chars_in_table), sizeof(long));

    for (long i = 0; i < num_chars_in_table; ++i) { // 读入header_数组
        ifp.read(reinterpret_cast<char*>(&header_[i].b), 1); // 字符本身
        unsigned char code_length_uc;
        ifp.read(reinterpret_cast<char*>(&code_length_uc), 1); // 编码长度
        long code_length = static_cast<long>(code_length_uc);
        
        header_[i].count = code_length; // 解压时count用来存储编码长度
        header_[i].bits = ""; // 清空编码字符串

        long bytes_to_read = (code_length + CHAR_BIT - 1) / CHAR_BIT; // 计算编码需要多少字节存储
        for (long j = 0; j < bytes_to_read; ++j) {
            unsigned char byte_read;
            ifp.read(reinterpret_cast<char*>(&byte_read), 1);
            header_[i].bits += byteToBinaryString(byte_read); // 将字节转为二进制字符串并追加
        }
        header_[i].bits = header_[i].bits.substr(0, code_length); // 根据实际编码长度截断
    }
    return num_chars_in_table;
}

void HuffmanDecompressor::sortHuffmanCodesByLength(long num_chars_in_table) {
    std::sort(header_, header_ + num_chars_in_table, [](const HuffmanNode& a, const HuffmanNode& b) {
        return a.bits.length() < b.bits.length(); // 按编码长度升序排列
    });
}

long HuffmanDecompressor::decodeAndWriteData(std::ifstream& ifp, std::ofstream& ofp, long original_file_length, long num_chars_in_table) {
    ifp.seekg(2 * sizeof(long), std::ios::beg); // 回到压缩数据开始的位置 (跳过两个long的头部)

    long decoded_chars_count = 0;
    std::string bit_buffer = "";
    size_t max_code_len = 0;

    // 找到最长哈夫曼编码的长度，用于优化读取缓冲区
    if (num_chars_in_table > 0) {
        max_code_len = header_[num_chars_in_table - 1].bits.length(); // 因为已经按长度排序，取最后一个
    } else {
        std::cerr << "Warning: Huffman code table is empty. Cannot decompress." << std::endl;
        return 0;
    }

    while (decoded_chars_count < original_file_length) {
        // 确保缓冲区足够长以匹配最长的编码，或者直到文件结束
        while (bit_buffer.length() < max_code_len && ifp.good()) {
            unsigned char byte_read;
            if (ifp.read(reinterpret_cast<char*>(&byte_read), 1)) {
                bit_buffer += byteToBinaryString(byte_read);
            } else {
                break; // 文件结束或读取错误
            }
        }

        bool found_match = false;
        // 优化：从短到长尝试匹配，因为表已经按长度排序。
        // 但这里直接遍历整个表也可以，因为map或vector的查找效率已经足够。
        for (long i = 0; i < num_chars_in_table; ++i) { // 遍历所有哈夫曼编码进行匹配
            const std::string& current_code = header_[i].bits;
            if (bit_buffer.length() >= current_code.length() && 
                bit_buffer.substr(0, current_code.length()) == current_code) {
                
                unsigned char decoded_char = header_[i].b;
                ofp.write(reinterpret_cast<const char*>(&decoded_char), 1);
                decoded_chars_count++;

                bit_buffer = bit_buffer.substr(current_code.length()); // 移除已解码的前缀
                found_match = true;
                break; // 找到一个字符，重新开始匹配
            }
        }

        if (!found_match) {
            // 如果缓冲区还有数据但无法匹配任何编码，可能文件损坏或数据不完整
            if (!bit_buffer.empty() && decoded_chars_count < original_file_length) {
                std::cerr << "Error: Could not decode Huffman prefix. File might be corrupted or incomplete." << std::endl;
            }
            break; // 无法解码，退出循环
        }
    }
    return decoded_chars_count;
}

void HuffmanDecompressor::decompress(const std::string& input_filepath, const std::string& output_filepath) {
    std::ifstream ifp(input_filepath, std::ios::binary);
    if (!ifp.is_open()) {
        throw std::runtime_error("Error: fail to open file: " + input_filepath);
    }


    std::ofstream ofp(output_filepath, std::ios::binary);
    if (!ofp.is_open()) {
        throw std::runtime_error("fail to decompress fiel, can not create output file: " + output_filepath);
    }

    // 1. 读取文件头部信息
    long original_file_length = 0;
    long huffman_table_start_pos = 0;
    readFileHeader(ifp, original_file_length, huffman_table_start_pos);

    // 2. 读取哈夫曼编码表
    long num_chars_in_table = readHuffmanTable(ifp, huffman_table_start_pos);
    if (num_chars_in_table <= 0) {
        std::cerr << "Huffman encoding table is empty or read failed, unable to decompress" << std::endl;
        ifp.close(); ofp.close(); return;
    }

    // 3. 对哈夫曼编码表进行排序 (按编码长度，以便解码时优先匹配短编码)
    sortHuffmanCodesByLength(num_chars_in_table);

    // 4. 解码并写入数据
    long decoded_actual_length = decodeAndWriteData(ifp, ofp, original_file_length, num_chars_in_table);

    ifp.close();
    ofp.close();

    std::cout << "解压缩文件成功！" << std::endl;
    std::cout << "原始文件长度: " << original_file_length << " 字节" << std::endl;
    std::cout << "解压缩后文件长度: " << decoded_actual_length << " 字节" << std::endl;
    if (decoded_actual_length == original_file_length) {
        std::cout << "解压缩文件与原文件相同！" << std::endl << std::endl;
    } else {
        std::cout << "解压缩文件与原文件不符！" << std::endl << std::endl;
    }
}