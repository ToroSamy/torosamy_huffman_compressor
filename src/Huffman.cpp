#include "Huffman.h"


std::string byteToBinaryString(unsigned char byte) {
    std::string binaryString;
    for (int i = CHAR_BIT - 1; i >= 0; --i) {
        binaryString += ((byte >> i) & 1) ? '1' : '0';
    }
    return binaryString;
}