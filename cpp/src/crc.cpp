#include "crc.h"
#include <random>

uint32_t computeCRC(const std::vector<uint8_t>& data) {
    uint32_t crc = 0xFFFFFFFF;
    for (uint8_t byte : data) {
        crc ^= byte;
        for (int i = 0; i < 8; i++) {
            crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
        }
    }
    return ~crc;
}

bool verifyCRC(const std::vector<uint8_t>& dataWithCRC) {
    if (dataWithCRC.size() < 4) return false;
    std::vector<uint8_t> data(dataWithCRC.begin(), dataWithCRC.end() - 4);
    uint32_t computedCRC = computeCRC(data);
    uint32_t receivedCRC = (dataWithCRC[dataWithCRC.size() - 4] << 24) |
                           (dataWithCRC[dataWithCRC.size() - 3] << 16) |
                           (dataWithCRC[dataWithCRC.size() - 2] << 8) |
                           dataWithCRC[dataWithCRC.size() - 1];
    return computedCRC == receivedCRC;
}

std::vector<uint8_t> simulateError(const std::vector<uint8_t>& data) {
    std::vector<uint8_t> corruptedData = data;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, corruptedData.size() * 8 - 1);
    int bitToFlip = dis(gen);
    int byteIndex = bitToFlip / 8;
    int bitIndex = bitToFlip % 8;
    corruptedData[byteIndex] ^= (1 << bitIndex);
    return corruptedData;
} 