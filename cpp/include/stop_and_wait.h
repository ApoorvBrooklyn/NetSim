#pragma once
#include <vector>
#include <cstdint>
#include <chrono>

// Configuration for the protocol
struct ProtocolConfig {
    int maxRetries = 3;                    // Maximum number of retransmissions
    int timeoutMs = 1000;                  // Timeout in milliseconds
    double errorProbability = 0.1;         // Probability of packet corruption
    double lossProbability = 0.1;          // Probability of packet loss
    double ackLossProbability = 0.1;       // Probability of ACK loss
};

// Send a packet with CRC, sequence number, and wait for ACK
bool sendPacket(const std::vector<uint8_t>& data, std::vector<uint8_t>& ack, int& seqNum, const ProtocolConfig& config = ProtocolConfig());

// Receive a packet, verify CRC and sequence number
bool receivePacket(std::vector<uint8_t>& data, int& seqNum, const ProtocolConfig& config = ProtocolConfig());

// Simulate network conditions (errors and losses, including ACK loss)
bool simulateNetworkConditions(const std::vector<uint8_t>& packet, const ProtocolConfig& config, bool isAck = false); 