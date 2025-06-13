#include "stop_and_wait.h"
#include <iostream>
#include <random>
#include <thread>
#include <chrono>

// CRC computation
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

// Simulate network conditions (errors, losses, and ACK loss)
bool simulateNetworkConditions(const std::vector<uint8_t>& packet, const ProtocolConfig& config, bool isAck) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);

    if (isAck) {
        if (dis(gen) < config.ackLossProbability) {
            std::cout << "ACK lost!" << std::endl;
            return false;
        }
    } else {
        // Simulate packet loss
        if (dis(gen) < config.lossProbability) {
            std::cout << "Packet lost!" << std::endl;
            return false;
        }
        // Simulate packet corruption
        if (dis(gen) < config.errorProbability) {
            std::cout << "Packet corrupted!" << std::endl;
            return false;
        }
    }
    return true;
}

bool sendPacket(const std::vector<uint8_t>& data, std::vector<uint8_t>& ack, int& seqNum, const ProtocolConfig& config) {
    // Add sequence number as the first byte
    std::vector<uint8_t> packet = {static_cast<uint8_t>(seqNum & 0xFF)};
    packet.insert(packet.end(), data.begin(), data.end());
    // Compute CRC for the data (including seqNum)
    uint32_t crc = computeCRC(packet);
    packet.push_back((crc >> 24) & 0xFF);
    packet.push_back((crc >> 16) & 0xFF);
    packet.push_back((crc >> 8) & 0xFF);
    packet.push_back(crc & 0xFF);

    int retries = 0;
    while (retries < config.maxRetries) {
        std::cout << "Attempt " << (retries + 1) << " of " << config.maxRetries << std::endl;
        std::cout << "Sending packet (seqNum=" << seqNum << ") with CRC: ";
        for (uint8_t byte : packet) {
            std::cout << std::hex << static_cast<int>(byte) << " ";
        }
        std::cout << std::dec << std::endl;

        // Simulate network conditions for data packet
        if (!simulateNetworkConditions(packet, config, false)) {
            std::cout << "Network error occurred, retrying..." << std::endl;
            retries++;
            continue;
        }

        // Simulate waiting for ACK with timeout
        auto start = std::chrono::steady_clock::now();
        while (true) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count();
            if (elapsed >= config.timeoutMs) {
                std::cout << "Timeout occurred!" << std::endl;
                retries++;
                break;
            }
            // Simulate receiving ACK (with sequence number)
            std::vector<uint8_t> ackPacket = {static_cast<uint8_t>(seqNum & 0xFF)};
            if (simulateNetworkConditions(ackPacket, config, true)) {
                ack = ackPacket;
                std::cout << "ACK received successfully! (seqNum=" << seqNum << ")" << std::endl;
                seqNum = (seqNum + 1) % 2; // Toggle sequence number for Stop-and-Wait
                return true;
            }
            // Small delay to prevent busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
    std::cout << "Max retries exceeded!" << std::endl;
    return false;
}

bool receivePacket(std::vector<uint8_t>& data, int& expectedSeqNum, const ProtocolConfig& config) {
    std::cout << "Receiving packet..." << std::endl;
    if (data.size() < 5) return false; // At least seqNum + CRC
    // Simulate network conditions for data packet
    if (!simulateNetworkConditions(data, config, false)) {
        std::cout << "Packet lost or corrupted during reception!" << std::endl;
        return false;
    }
    // Extract sequence number
    int receivedSeqNum = data[0];
    // Verify CRC
    std::vector<uint8_t> packetData(data.begin(), data.end() - 4);
    uint32_t computedCRC = computeCRC(packetData);
    uint32_t receivedCRC = (data[data.size() - 4] << 24) |
                           (data[data.size() - 3] << 16) |
                           (data[data.size() - 2] << 8) |
                           data[data.size() - 1];
    bool isValid = computedCRC == receivedCRC;
    std::cout << "CRC Verification: " << (isValid ? "Valid" : "Invalid") << std::endl;
    if (!isValid) return false;
    // Check sequence number
    if (receivedSeqNum != expectedSeqNum) {
        std::cout << "Unexpected sequence number! Expected: " << expectedSeqNum << ", Got: " << receivedSeqNum << std::endl;
        return false;
    }
    // Remove sequence number from data
    data = std::vector<uint8_t>(data.begin() + 1, data.end() - 4);
    std::cout << "Packet received successfully! (seqNum=" << receivedSeqNum << ")" << std::endl;
    expectedSeqNum = (expectedSeqNum + 1) % 2;
    return true;
} 