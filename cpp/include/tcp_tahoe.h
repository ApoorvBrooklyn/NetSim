#pragma once
#include <vector>
#include <cstdint>
#include <chrono>
#include <deque>
#include <map>

// TCP Tahoe State
enum class TCPState {
    SLOW_START,
    CONGESTION_AVOIDANCE,
    FAST_RECOVERY
};

// TCP Tahoe Configuration
struct TCPConfig {
    uint32_t initialWindowSize = 1;        // Initial congestion window size (in segments)
    uint32_t maxWindowSize = 64;          // Maximum window size
    uint32_t mss = 1460;                  // Maximum Segment Size (bytes)
    uint32_t initialRTT = 100;            // Initial Round Trip Time (ms)
    uint32_t timeout = 1000;              // Timeout duration (ms)
    double lossProbability = 0.1;         // Probability of packet loss
    uint32_t ssthresh = 16;               // Slow Start Threshold
    uint32_t windowScale = 1;             // Window scaling factor
    uint32_t delayedAckTimeout = 200;     // Delayed ACK timeout (ms)
    uint32_t maxDelayedAcks = 2;          // Maximum number of segments to wait before sending ACK
};

// TCP Tahoe Statistics
struct TCPStats {
    uint32_t totalPacketsSent = 0;
    uint32_t totalPacketsLost = 0;
    uint32_t totalRetransmissions = 0;
    uint32_t currentWindowSize = 0;
    uint32_t currentRTT = 0;
    uint32_t srtt = 0;                    // Smoothed Round Trip Time
    uint32_t rttvar = 0;                  // RTT Variation
    uint32_t rto = 0;                     // Retransmission Timeout
    uint32_t totalBytesSent = 0;
    uint32_t totalBytesAcked = 0;
    uint32_t totalDelayedAcks = 0;
    uint32_t totalFastRetransmits = 0;
    TCPState currentState = TCPState::SLOW_START;
    std::map<uint32_t, uint32_t> windowSizeHistory;  // Window size over time
};

class TCPTahoe {
public:
    TCPTahoe(const TCPConfig& config = TCPConfig());
    
    // Core TCP Tahoe functions
    bool sendData(const std::vector<uint8_t>& data);
    bool receiveAck(uint32_t ackNumber);
    void handleTimeout();
    void handlePacketLoss();
    
    // State management
    void enterSlowStart();
    void enterCongestionAvoidance();
    void enterFastRecovery();
    
    // Statistics and monitoring
    TCPStats getStats() const;
    void resetStats();
    std::vector<uint32_t> getWindowSizeHistory() const;
    
private:
    TCPConfig config;
    TCPStats stats;
    TCPState state;
    
    uint32_t cwnd;                // Congestion window size
    uint32_t ssthresh;            // Slow start threshold
    uint32_t dupAckCount;         // Duplicate ACK counter
    uint32_t nextSeqNum;          // Next sequence number to send
    uint32_t lastAckedSeq;        // Last acknowledged sequence number
    
    // RTT estimation
    uint32_t srtt;                // Smoothed Round Trip Time
    uint32_t rttvar;              // RTT Variation
    uint32_t rto;                 // Retransmission Timeout
    
    // Delayed ACK handling
    uint32_t delayedAckCount;     // Number of segments received without ACK
    std::chrono::steady_clock::time_point lastAckTime;
    
    std::deque<std::vector<uint8_t>> sendBuffer;  // Buffer for unacknowledged packets
    std::map<uint32_t, std::chrono::steady_clock::time_point> packetSendTimes;  // Track send times for RTT calculation
    
    std::chrono::steady_clock::time_point lastSendTime;
    
    // Helper functions
    void updateWindowSize();
    bool isTimeout();
    void retransmit();
    void updateRTT(uint32_t rtt);
    void handleDelayedAck();
    void recordWindowSize();
}; 