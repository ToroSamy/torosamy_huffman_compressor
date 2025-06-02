#include "HuffmanCompressor.h"
#include <iostream>
#include <algorithm> 
#include <climits> 
#include <stdexcept>
#include <filesystem> 
#include <map>
#include <vector>

HuffmanCompressor::HuffmanCompressor() {
}

long HuffmanCompressor::calculateFrequencies(std::ifstream& ifp) {
    long file_length = 0;
    unsigned char c;

    // 初始化header_数组
    for (int i = 0; i < 512; ++i) {
        header_[i] = HuffmanNode(); // 调用默认构造函数
        header_[i].b = static_cast<unsigned char>(i); // 为前256个字符节点设置字符值
    }

    // 逐字节读取文件，统计频率
    ifp.clear(); // 清除可能的EOF标志
    ifp.seekg(0, std::ios::beg); // 确保从文件开头读取

    while (ifp.read(reinterpret_cast<char*>(&c), 1)) {
        header_[c].count++;
        file_length++;
    }
    return file_length;
}

void HuffmanCompressor::sortNodesByFrequency(long num_distinct_chars) {
    // 对 header_数组中的对象直接操作
    std::sort(header_, header_ + num_distinct_chars, [](const HuffmanNode& a, const HuffmanNode& b) {
        return a.count > b.count; // 降序排列
    });
}

long HuffmanCompressor::buildHuffmanTree(long num_distinct_chars) {
    if (num_distinct_chars <= 1) { // 0或1个字符不需要构建树
        if (num_distinct_chars == 1) { // 如果只有一个字符，给它一个默认编码 '0'
            header_[0].bits = "0";
        }
        return num_distinct_chars;
    }

    // 复制前 num_distinct_chars 个有效节点到 vector，方便操作
    std::vector<HuffmanNode*> active_nodes_ptrs;
    for (long i = 0; i < num_distinct_chars; ++i) {
        active_nodes_ptrs.push_back(&header_[i]);
    }

    long next_available_node_idx = num_distinct_chars; // 从 num_distinct_chars 之后开始存放内部节点

    // 只要待合并的节点数量大于 1，就继续合并
    while (active_nodes_ptrs.size() > 1) {
        // 找到频率最小的两个节点
        auto it1 = std::min_element(active_nodes_ptrs.begin(), active_nodes_ptrs.end(), [](const HuffmanNode* a, const HuffmanNode* b) {
            return a->count < b->count;
        });
        HuffmanNode* node1 = *it1;
        active_nodes_ptrs.erase(it1);

        auto it2 = std::min_element(active_nodes_ptrs.begin(), active_nodes_ptrs.end(), [](const HuffmanNode* a, const HuffmanNode* b) {
            return a->count < b->count;
        });
        HuffmanNode* node2 = *it2;
        active_nodes_ptrs.erase(it2);

        // 创建新的父节点
        if (next_available_node_idx >= 512) {
             throw std::runtime_error("Huffman tree construction error: Header array size exceeded.");
        }
        header_[next_available_node_idx].count = node1->count + node2->count;
        // 约定：左子节点的字符值较小
        header_[next_available_node_idx].lch = (node1->b < node2->b) ? (node1 - header_) : (node2 - header_);
        header_[next_available_node_idx].rch = (node1->b < node2->b) ? (node2 - header_) : (node1 - header_);

        // 设置子节点的父节点
        node1->parent = next_available_node_idx;
        node2->parent = next_available_node_idx;

        // 将新创建的父节点加入到 active_nodes_ptrs 中
        active_nodes_ptrs.push_back(&header_[next_available_node_idx]);
        next_available_node_idx++;
    }
    return next_available_node_idx; // 返回实际使用的总节点数 (包括叶子和内部节点)
}

void HuffmanCompressor::generateHuffmanCodes(long num_distinct_chars) {
    for (long i = 0; i < num_distinct_chars; ++i) { // 遍历所有叶子节点
        long current_node_idx = i;
        std::string current_code_str = "";

        while (header_[current_node_idx].parent != -1) {
            long parent_idx = header_[current_node_idx].parent;
            if (header_[parent_idx].lch == current_node_idx) { // 如果是左子节点
                current_code_str = '0' + current_code_str;
            } else { // 如果是右子节点
                current_code_str = '1' + current_code_str;
            }
            current_node_idx = parent_idx;
        }
        header_[i].bits = current_code_str; // 存储编码
    }
}

long HuffmanCompressor::writeCompressedData(std::ifstream& ifp, std::ofstream& ofp, long original_file_length, long num_distinct_chars) {
    // 写入原始文件长度
    ofp.write(reinterpret_cast<const char*>(&original_file_length), sizeof(long));
    // 写入哈夫曼表起始位置占位符
    long header_table_start_pos_placeholder = 0;
    ofp.write(reinterpret_cast<const char*>(&header_table_start_pos_placeholder), sizeof(long));
    
    // 回到输入文件开头，准备读取数据进行压缩
    ifp.clear();
    ifp.seekg(0, std::ios::beg);

    std::string bit_buffer = "";
    long compressed_bytes_count = 0;
    unsigned char c;

    // 构建一个 map 方便快速查找字符对应的编码，因为它可能在 header_数组中被移动了
    std::map<unsigned char, std::string> huffman_codes_map;
    for(long i = 0; i < num_distinct_chars; ++i) {
        huffman_codes_map[header_[i].b] = header_[i].bits;
    }

    while (ifp.read(reinterpret_cast<char*>(&c), 1)) {
        if (huffman_codes_map.count(c)) {
            bit_buffer += huffman_codes_map.at(c);
        } else {
            std::cerr << "Warning: Character " << static_cast<int>(c) << " (0x" << std::hex << (int)c << std::dec << ") not found in Huffman codes during compression. Skipping." << std::endl;
            continue;
        }

        while (bit_buffer.length() >= CHAR_BIT) {
            unsigned char byte_to_write = 0;
            for (int i = 0; i < CHAR_BIT; ++i) {
                if (bit_buffer[i] == '1') byte_to_write = (byte_to_write << 1) | 1;
                else byte_to_write = byte_to_write << 1;
            }
            ofp.write(reinterpret_cast<const char*>(&byte_to_write), 1);
            compressed_bytes_count++;
            bit_buffer = bit_buffer.substr(CHAR_BIT);
        }
    }

    // 处理剩余的位
    if (!bit_buffer.empty()) {
        unsigned char byte_to_write = 0;
        for (size_t i = 0; i < bit_buffer.length(); ++i) {
            if (bit_buffer[i] == '1') byte_to_write = (byte_to_write << 1) | 1;
            else byte_to_write = byte_to_write << 1;
        }
        byte_to_write <<= (CHAR_BIT - bit_buffer.length()); // 低位补0
        ofp.write(reinterpret_cast<const char*>(&byte_to_write), 1);
        compressed_bytes_count++;
    }
    return compressed_bytes_count;
}

void HuffmanCompressor::writeHuffmanTable(std::ofstream& ofp, long num_distinct_chars) {
    // 回填哈夫曼表起始位置
    long current_pos_after_data = ofp.tellp(); // 获取当前位置
    ofp.seekp(sizeof(long), std::ios::beg); // 回到文件头，写入哈夫曼表起始位置 (第二个long)
    ofp.write(reinterpret_cast<const char*>(&current_pos_after_data), sizeof(long));
    ofp.seekp(current_pos_after_data, std::ios::beg); // 回到之前的位置准备写入哈夫曼表

    // 写入有效字符的数量
    ofp.write(reinterpret_cast<const char*>(&num_distinct_chars), sizeof(long));

    for (long i = 0; i < num_distinct_chars; ++i) { // 遍历所有有效字符
        ofp.write(reinterpret_cast<const char*>(&header_[i].b), 1); // 写入字符本身
        unsigned char code_length = static_cast<unsigned char>(header_[i].bits.length());
        ofp.write(reinterpret_cast<const char*>(&code_length), 1); // 写入编码长度

        // 写入编码的字节序列
        const std::string& current_code_to_write = header_[i].bits;
        size_t bit_idx = 0;
        while (bit_idx < current_code_to_write.length()) {
            unsigned char byte_to_write = 0;
            for (int bit_pos = 0; bit_pos < CHAR_BIT; ++bit_pos) {
                if (bit_idx < current_code_to_write.length()) {
                    if (current_code_to_write[bit_idx] == '1') byte_to_write = (byte_to_write << 1) | 1;
                    else byte_to_write = byte_to_write << 1;
                    bit_idx++;
                } else {
                    byte_to_write = byte_to_write << 1; // 补0
                }
            }
            ofp.write(reinterpret_cast<const char*>(&byte_to_write), 1);
        }
    }
}

void HuffmanCompressor::compress(const std::string& input_filepath, const std::string& output_filepath) {
    std::ifstream ifp(input_filepath, std::ios::binary);
    if (!ifp.is_open()) {
        throw std::runtime_error("文件打开失败: " + input_filepath);
    }

 
    std::ofstream ofp(output_filepath, std::ios::binary);
    if (!ofp.is_open()) {
        throw std::runtime_error("压缩文件失败，无法创建输出文件: " + output_filepath);
    }

    // 1. 统计字符频率
    long original_file_length = calculateFrequencies(ifp);
    if (original_file_length == 0) {
        std::cout << "输入文件为空或不含可压缩内容，无需压缩。" << std::endl;
        ifp.close(); ofp.close(); return;
    }

    // 2. 确定实际有多少种不同的字符
    long num_distinct_chars = 0;
    for (int i = 0; i < 256; ++i) {
        if (header_[i].count > 0) {
            num_distinct_chars++;
        }
    }

    if (num_distinct_chars == 0) {
        std::cout << "文件中不包含任何可压缩字符" << std::endl;
        ifp.close(); ofp.close(); return;
    }
    
    // 3. 将有频率的字符节点移动到 header_数组的前面，并按频率降序排列
    // 这一步是关键，确保后续的树构建只处理有效字符
    std::vector<HuffmanNode> temp_active_nodes;
    for (int i = 0; i < 256; ++i) {
        if (header_[i].count > 0) {
            temp_active_nodes.push_back(header_[i]);
        }
    }
    std::sort(temp_active_nodes.begin(), temp_active_nodes.end(), [](const HuffmanNode& a, const HuffmanNode& b) {
        return a.count > b.count; // 降序
    });
    // 将排序后的节点复制回 header_数组的前部
    for (long i = 0; i < temp_active_nodes.size(); ++i) {
        header_[i] = temp_active_nodes[i];
        header_[i].parent = -1; // 确保父节点重置
        header_[i].lch = -1;
        header_[i].rch = -1;
        header_[i].bits = ""; // 清空编码
    }
    // 清空 header_数组中超出实际字符数量的部分（可选，但安全起见）
    for (long i = temp_active_nodes.size(); i < 512; ++i) {
        header_[i] = HuffmanNode(); // 重置为默认状态
    }

    // 4. 构建哈夫曼树
    long total_nodes_in_tree = buildHuffmanTree(num_distinct_chars);
    if (total_nodes_in_tree == -1 && num_distinct_chars > 1) { // 只有多于一个字符才需要严格的树构建
        throw std::runtime_error("构建哈夫曼树失败，无法压缩。");
    }

    // 5. 生成哈夫曼编码
    generateHuffmanCodes(num_distinct_chars);

    // 6. 写入压缩数据和头部信息（包括原始长度和哈夫曼表起始位置占位）
    writeCompressedData(ifp, ofp, original_file_length, num_distinct_chars);
    
    // 7. 写入哈夫曼编码表 (包括回填哈夫曼表起始位置)
    writeHuffmanTable(ofp, num_distinct_chars);

    ifp.close();
    ofp.close();

    // 8. 计算压缩率
    long final_compressed_file_length = std::filesystem::file_size(output_filepath);
    double compression_ratio = 0.0;
    if (original_file_length > 0) {
        compression_ratio = (static_cast<double>(original_file_length) - final_compressed_file_length) / original_file_length;
    }
    
    std::cout << "压缩文件成功！" << std::endl;
    std::cout << "原始文件大小: " << original_file_length << " 字节" << std::endl;
    std::cout << "压缩后文件大小: " << final_compressed_file_length << " 字节" << std::endl;
    std::cout << "压缩率为 " << compression_ratio * 100 << "%" << std::endl << std::endl;
}