#include "tcp_tahoe.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdlib>

// Constants for RTT estimation
constexpr uint32_t ALPHA = 0.125;  // Weight for SRTT
constexpr uint32_t BETA = 0.25;    // Weight for RTTVAR
constexpr uint32_t K = 4;          // RTO multiplier

TCPTahoe::TCPTahoe(const TCPConfig& config) 
    : config(config), state(TCPState::SLOW_START), cwnd(config.initialWindowSize),
      ssthresh(config.ssthresh), dupAckCount(0), nextSeqNum(0), lastAckedSeq(0),
      srtt(config.initialRTT), rttvar(config.initialRTT / 2), rto(config.initialRTT + K * rttvar),
      delayedAckCount(0) {
    resetStats();
}

bool TCPTahoe::sendData(const std::vector<uint8_t>& data) {
    // Check if we can send more data based on congestion window
    if (sendBuffer.size() >= cwnd * config.windowScale) {
        std::cout << "Congestion window full, waiting..." << std::endl;
        return false;
    }

    // Simulate packet loss
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    
    if (dis(gen) < config.lossProbability) {
        std::cout << "Packet lost during transmission!" << std::endl;
        stats.totalPacketsLost++;
        handlePacketLoss();
        return false;
    }

    // Add sequence number to packet
    std::vector<uint8_t> packet = data;
    packet.insert(packet.begin(), (nextSeqNum >> 24) & 0xFF);
    packet.insert(packet.begin() + 1, (nextSeqNum >> 16) & 0xFF);
    packet.insert(packet.begin() + 2, (nextSeqNum >> 8) & 0xFF);
    packet.insert(packet.begin() + 3, nextSeqNum & 0xFF);

    // Add to send buffer and record send time
    sendBuffer.push_back(packet);
    auto now = std::chrono::steady_clock::now();
    packetSendTimes[nextSeqNum] = now;
    lastSendTime = now;

    nextSeqNum += data.size();
    stats.totalPacketsSent++;
    stats.totalBytesSent += data.size();
    stats.currentWindowSize = cwnd;
    
    // Record window size for visualization
    recordWindowSize();

    std::cout << "Sent packet with seq=" << (nextSeqNum - data.size()) 
              << ", cwnd=" << cwnd * config.windowScale << ", state=" 
              << (state == TCPState::SLOW_START ? "SLOW_START" : 
                  state == TCPState::CONGESTION_AVOIDANCE ? "CONGESTION_AVOIDANCE" : "FAST_RECOVERY")
              << std::endl;

    return true;
}

bool TCPTahoe::receiveAck(uint32_t ackNumber) {
    if (ackNumber <= lastAckedSeq) {
        // Duplicate ACK
        dupAckCount++;
        std::cout << "Received duplicate ACK " << ackNumber << " (count: " << dupAckCount << ")" << std::endl;
        
        if (dupAckCount == 3) {
            // Fast Retransmit
            std::cout << "Fast Retransmit triggered!" << std::endl;
            ssthresh = cwnd / 2;
            cwnd = 1;
            enterFastRecovery();
            retransmit();
            stats.totalFastRetransmits++;
        }
        return false;
    }

    // New ACK received
    dupAckCount = 0;
    
    // Calculate RTT for this ACK
    auto now = std::chrono::steady_clock::now();
    auto it = packetSendTimes.find(ackNumber);
    if (it != packetSendTimes.end()) {
        uint32_t rtt = std::chrono::duration_cast<std::chrono::milliseconds>(
            now - it->second).count();
        updateRTT(rtt);
        packetSendTimes.erase(it);
    }

    // Remove acknowledged packets from buffer
    while (!sendBuffer.empty() && 
           (sendBuffer.front()[0] << 24 | sendBuffer.front()[1] << 16 | 
            sendBuffer.front()[2] << 8 | sendBuffer.front()[3]) < ackNumber) {
        stats.totalBytesAcked += sendBuffer.front().size() - 4;  // Subtract sequence number bytes
        sendBuffer.pop_front();
    }

    lastAckedSeq = ackNumber;

    // Update congestion window based on state
    switch (state) {
        case TCPState::SLOW_START:
            cwnd += 1;
            if (cwnd >= ssthresh) {
                enterCongestionAvoidance();
            }
            break;
            
        case TCPState::CONGESTION_AVOIDANCE:
            cwnd += 1.0 / cwnd;
            break;
            
        case TCPState::FAST_RECOVERY:
            cwnd = ssthresh;
            enterCongestionAvoidance();
            break;
    }

    // Ensure window size doesn't exceed maximum
    cwnd = std::min(cwnd, config.maxWindowSize);
    stats.currentWindowSize = cwnd;
    
    // Record window size for visualization
    recordWindowSize();

    std::cout << "Received ACK " << ackNumber << ", new cwnd=" << cwnd * config.windowScale 
              << ", RTT=" << stats.currentRTT << "ms" << std::endl;
    return true;
}

void TCPTahoe::handleTimeout() {
    std::cout << "Timeout occurred!" << std::endl;
    ssthresh = cwnd / 2;
    cwnd = 1;
    enterSlowStart();
    retransmit();
    stats.totalRetransmissions++;
    
    // Double the RTO (exponential backoff)
    rto *= 2;
}

void TCPTahoe::handlePacketLoss() {
    ssthresh = cwnd / 2;
    cwnd = 1;
    enterSlowStart();
    stats.totalPacketsLost++;
}

void TCPTahoe::enterSlowStart() {
    state = TCPState::SLOW_START;
    std::cout << "Entering Slow Start" << std::endl;
}

void TCPTahoe::enterCongestionAvoidance() {
    state = TCPState::CONGESTION_AVOIDANCE;
    std::cout << "Entering Congestion Avoidance" << std::endl;
}

void TCPTahoe::enterFastRecovery() {
    state = TCPState::FAST_RECOVERY;
    std::cout << "Entering Fast Recovery" << std::endl;
}

TCPStats TCPTahoe::getStats() const {
    return stats;
}

void TCPTahoe::resetStats() {
    stats = TCPStats();
    stats.currentState = state;
    stats.srtt = srtt;
    stats.rttvar = rttvar;
    stats.rto = rto;
}

void TCPTahoe::updateWindowSize() {
    stats.currentWindowSize = cwnd;
}

bool TCPTahoe::isTimeout() {
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastSendTime).count();
    return elapsed > rto;
}

void TCPTahoe::retransmit() {
    if (!sendBuffer.empty()) {
        std::cout << "Retransmitting packet with seq=" 
                  << (sendBuffer.front()[0] << 24 | sendBuffer.front()[1] << 16 | 
                      sendBuffer.front()[2] << 8 | sendBuffer.front()[3])
                  << std::endl;
        lastSendTime = std::chrono::steady_clock::now();
    }
}

void TCPTahoe::updateRTT(uint32_t rtt) {
    // Update SRTT and RTTVAR using Jacobson's algorithm
    if (srtt == 0) {
        srtt = rtt;
        rttvar = rtt / 2;
    } else {
        rttvar = (1 - BETA) * rttvar + BETA * std::abs(static_cast<int64_t>(srtt) - static_cast<int64_t>(rtt));
        srtt = (1 - ALPHA) * srtt + ALPHA * rtt;
    }
    
    // Update RTO
    rto = srtt + K * rttvar;
    
    // Update statistics
    stats.currentRTT = rtt;
    stats.srtt = srtt;
    stats.rttvar = rttvar;
    stats.rto = rto;
}

void TCPTahoe::handleDelayedAck() {
    if (delayedAckCount >= config.maxDelayedAcks) {
        std::cout << "Sending delayed ACK" << std::endl;
        stats.totalDelayedAcks++;
        delayedAckCount = 0;
    }
}

void TCPTahoe::recordWindowSize() {
    auto now = std::chrono::steady_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    stats.windowSizeHistory[timestamp] = cwnd * config.windowScale;
}

std::vector<uint32_t> TCPTahoe::getWindowSizeHistory() const {
    std::vector<uint32_t> history;
    for (const auto& pair : stats.windowSizeHistory) {
        history.push_back(pair.second);
    }
    return history;
} 