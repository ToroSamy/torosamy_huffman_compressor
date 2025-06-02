#include <iostream>
#include <string>
#include <stdexcept>
#include "HuffmanCompressor.h"
#include "HuffmanDecompressor.h"
#include <limits>
void showMenu() {
    std::cout <<"0.退出程序"<<std::endl;
    std::cout <<"1.压缩文件"<<std::endl;
    std::cout <<"2.解压文件"<<std::endl;
    std::cout <<"请输入选项: ";
}

int safeChoose() {
    int i = 0;
    while(std::cin >> i, !std::cin.eof()) {
        if (std::cin.bad()) {
            throw std::runtime_error("cin is corrupted");
        }
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "data format error, please try again" << std::endl;
            continue;
        }
        return i;
    }
    return i;
}


// --- main 函数，用于测试 ---
int main() {
    while (true) {
        showMenu();
        int option = safeChoose();

        if (option == 0) {
            exit(0);
        }

        if (option == 1) {
            std::string input_file_name, output_file_name;
            std::cout <<"请输入需要压缩的文件(如: /opt/example.java): ";
            std::cin>>input_file_name;
            std::cout <<"请输入压缩后的文件(如: /opt/example.torosamy): ";
            std::cin>>output_file_name;
            HuffmanCompressor compressor;
            compressor.compress(input_file_name, output_file_name);
            continue;
        }

        if (option == 2) {
            std::string input_file_name, output_file_name;
            std::cout << "请输入需要解压的文件(如: /opt/example.torosamy): ";
            std::cin>>input_file_name;
            std::cout << "请输入需要解压后的文件(如: /opt/output_example.java): ";
            std::cin>>output_file_name;
            HuffmanDecompressor decompressor;
            decompressor.decompress(input_file_name,output_file_name);
            continue;
        }

        std::cout << "不合法的输入. 请重新输入"<<std::endl;
    }

    return 0;
}
