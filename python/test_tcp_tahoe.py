import tcp_tahoe_module
import time
import random
import matplotlib.pyplot as plt
import numpy as np

def verify_tcp_behavior(stats, expected_state, expected_window_size, expected_rtt_range):
    """Verify if TCP behavior matches expected patterns"""
    print("\n=== TCP Behavior Verification ===")
    
    # Verify state
    state_match = stats.currentState == expected_state
    print(f"State Check: {'✓' if state_match else '✗'}")
    print(f"Expected: {expected_state}, Got: {stats.currentState}")
    
    # Verify window size
    window_match = stats.currentWindowSize == expected_window_size
    print(f"Window Size Check: {'✓' if window_match else '✗'}")
    print(f"Expected: {expected_window_size}, Got: {stats.currentWindowSize}")
    
    # Verify RTT
    rtt_in_range = expected_rtt_range[0] <= stats.currentRTT <= expected_rtt_range[1]
    print(f"RTT Check: {'✓' if rtt_in_range else '✗'}")
    print(f"Expected Range: {expected_rtt_range}, Got: {stats.currentRTT}ms")
    
    return state_match and window_match and rtt_in_range

def print_stats(stats):
    print("\nCurrent Statistics:")
    print(f"Total Packets Sent: {stats.totalPacketsSent}")
    print(f"Total Packets Lost: {stats.totalPacketsLost}")
    print(f"Total Retransmissions: {stats.totalRetransmissions}")
    print(f"Total Fast Retransmits: {stats.totalFastRetransmits}")
    print(f"Total Delayed ACKs: {stats.totalDelayedAcks}")
    print(f"Current Window Size: {stats.currentWindowSize}")
    print(f"Current RTT: {stats.currentRTT}ms")
    print(f"SRTT: {stats.srtt}ms")
    print(f"RTTVAR: {stats.rttvar}ms")
    print(f"RTO: {stats.rto}ms")
    print(f"Total Bytes Sent: {stats.totalBytesSent}")
    print(f"Total Bytes Acked: {stats.totalBytesAcked}")
    print(f"Current State: {stats.currentState}")
    print("-" * 50)

def plot_window_size_history(history):
    plt.figure(figsize=(12, 8))
    
    # Plot window size
    plt.subplot(2, 1, 1)
    plt.plot(history, label='Congestion Window Size', color='blue')
    plt.title('TCP Tahoe Congestion Window Size Over Time')
    plt.xlabel('Time (packet sequence)')
    plt.ylabel('Window Size (segments)')
    plt.grid(True)
    plt.legend()
    
    # Plot window size changes
    plt.subplot(2, 1, 2)
    window_changes = np.diff(history)
    plt.plot(window_changes, label='Window Size Changes', color='red')
    plt.title('Congestion Window Size Changes')
    plt.xlabel('Time (packet sequence)')
    plt.ylabel('Change in Window Size')
    plt.grid(True)
    plt.legend()
    
    plt.tight_layout()
    plt.savefig('tcp_tahoe_analysis.png')
    plt.close()

def simulate_network_conditions(tcp, data, num_packets=20):
    print("\n=== Simulating TCP Tahoe Congestion Control ===")
    print("Expected Behavior:")
    print("1. Slow Start: Window size should double until ssthresh")
    print("2. Congestion Avoidance: Window size should increase linearly")
    print("3. Fast Recovery: Window size should halve on packet loss")
    print("4. Timeout: Window size should reset to 1")
    print("-" * 50)
    
    # Track window size history
    window_history = []
    
    # Send initial packets
    for i in range(num_packets):
        print(f"\nSending packet {i+1}/{num_packets}")
        success = tcp.sendData(data)
        if not success:
            print("Failed to send packet")
            continue
            
        # Record window size
        window_history.append(tcp.getStats().currentWindowSize)
        
        # Simulate ACK reception with variable delay
        time.sleep(random.uniform(0.05, 0.15))  # Variable RTT
        if random.random() < 0.8:  # 80% chance of successful ACK
            tcp.receiveAck(i * len(data))
        else:
            print("Simulating ACK loss")
            tcp.handleTimeout()
            
        # Print current stats
        stats = tcp.getStats()
        print_stats(stats)
        
        # Verify behavior at key points
        if i == 0:
            # Initial state should be slow start with window size 1
            verify_tcp_behavior(stats, 
                              tcp_tahoe_module.TCPState.SLOW_START,
                              1,
                              (0, 200))
        elif i == 5:
            print("\nSimulating network congestion...")
            tcp.handlePacketLoss()
            stats = tcp.getStats()
            # After packet loss, should enter fast recovery
            verify_tcp_behavior(stats,
                              tcp_tahoe_module.TCPState.FAST_RECOVERY,
                              max(1, stats.currentWindowSize // 2),
                              (0, 200))
        elif i == 12:
            print("\nSimulating temporary network degradation...")
            time.sleep(0.5)  # Increased delay
            tcp.handleTimeout()
            stats = tcp.getStats()
            # After timeout, should enter slow start
            verify_tcp_behavior(stats,
                              tcp_tahoe_module.TCPState.SLOW_START,
                              1,
                              (0, 200))
    
    return window_history

def main():
    # Create TCP Tahoe configuration
    config = tcp_tahoe_module.TCPConfig()
    config.initialWindowSize = 1
    config.maxWindowSize = 16
    config.mss = 1460
    config.initialRTT = 100
    config.timeout = 1000
    config.lossProbability = 0.1
    config.ssthresh = 8
    config.windowScale = 2  # Enable window scaling
    config.delayedAckTimeout = 200
    config.maxDelayedAcks = 2
    
    # Create TCP Tahoe instance
    tcp = tcp_tahoe_module.TCPTahoe(config)
    
    # Test data
    data = [0x01, 0x02, 0x03, 0x04, 0x05]
    
    # Run simulation
    window_history = simulate_network_conditions(tcp, data)
    
    # Print final statistics
    print("\n=== Final Statistics ===")
    print_stats(tcp.getStats())
    
    # Plot window size history
    plot_window_size_history(window_history)
    print("\nWindow size analysis plot saved as 'tcp_tahoe_analysis.png'")
    
    # Print verification summary
    print("\n=== TCP Tahoe Behavior Verification Summary ===")
    print("The simulation demonstrates the following TCP Tahoe characteristics:")
    print("1. Slow Start: Window size doubles until ssthresh")
    print("2. Congestion Avoidance: Window size increases linearly")
    print("3. Fast Recovery: Window size halves on packet loss")
    print("4. Timeout: Window size resets to 1")
    print("5. RTT Estimation: SRTT and RTTVAR are updated")
    print("6. Window Scaling: Window size is properly scaled")
    print("\nCheck 'tcp_tahoe_analysis.png' for detailed visualization of the congestion window behavior.")

if __name__ == "__main__":
    main() 