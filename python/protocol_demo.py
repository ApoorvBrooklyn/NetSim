import crc_module
import stop_and_wait_module
import tcp_tahoe_module
import matplotlib.pyplot as plt
import numpy as np
import time
import random
from matplotlib.animation import FuncAnimation
import threading
import queue

class ProtocolDemo:
    def __init__(self):
        # TCP Tahoe configuration
        tcp_config = tcp_tahoe_module.TCPConfig()
        tcp_config.initialWindowSize = 1
        tcp_config.maxWindowSize = 16
        tcp_config.mss = 1460
        tcp_config.initialRTT = 100
        tcp_config.timeout = 1000
        tcp_config.lossProbability = 0.1
        tcp_config.ssthresh = 8
        tcp_config.windowScale = 2
        tcp_config.delayedAckTimeout = 200
        tcp_config.maxDelayedAcks = 2
        self.tcp = tcp_tahoe_module.TCPTahoe(tcp_config)
        
        # Stop-and-Wait config
        self.sw_config = stop_and_wait_module.ProtocolConfig()
        self.sw_config.maxRetries = 5
        self.sw_config.timeoutMs = 300
        self.sw_config.errorProbability = 0.1
        self.sw_config.lossProbability = 0.1
        self.sw_config.ackLossProbability = 0.05
        
        # Data queues for animation
        self.crc_queue = queue.Queue()
        self.stop_wait_queue = queue.Queue()
        self.tcp_queue = queue.Queue()
        
        # Initialize plots
        self.setup_plots()
        
        # Initialize history lists
        self.tcp_window_history = []
        self.tcp_rtt_history = []
        self.tcp_state_history = []
        self.tcp_throughput_history = []
        
    def setup_plots(self):
        # Create figure with subplots
        self.fig = plt.figure(figsize=(15, 10))
        
        # CRC Plot
        self.crc_ax = self.fig.add_subplot(331)
        self.crc_ax.set_title('CRC Calculation')
        self.crc_ax.set_xlabel('Data')
        self.crc_ax.set_ylabel('CRC Value')
        
        # Stop and Wait Plot
        self.stop_wait_ax = self.fig.add_subplot(332)
        self.stop_wait_ax.set_title('Stop and Wait Protocol')
        self.stop_wait_ax.set_xlabel('Time')
        self.stop_wait_ax.set_ylabel('Packet Status')
        
        # TCP Window Size Plot
        self.tcp_window_ax = self.fig.add_subplot(333)
        self.tcp_window_ax.set_title('TCP Tahoe Window Size')
        self.tcp_window_ax.set_xlabel('Time')
        self.tcp_window_ax.set_ylabel('Window Size')
        
        # TCP RTT Plot
        self.tcp_rtt_ax = self.fig.add_subplot(334)
        self.tcp_rtt_ax.set_title('TCP Tahoe RTT')
        self.tcp_rtt_ax.set_xlabel('Time')
        self.tcp_rtt_ax.set_ylabel('RTT (ms)')
        
        # TCP State Plot
        self.tcp_state_ax = self.fig.add_subplot(335)
        self.tcp_state_ax.set_title('TCP Tahoe State')
        self.tcp_state_ax.set_xlabel('Time')
        self.tcp_state_ax.set_ylabel('State')
        
        # TCP Throughput Plot
        self.tcp_throughput_ax = self.fig.add_subplot(336)
        self.tcp_throughput_ax.set_title('TCP Tahoe Throughput')
        self.tcp_throughput_ax.set_xlabel('Time')
        self.tcp_throughput_ax.set_ylabel('Bytes/s')
        
        plt.tight_layout()
        
    def demonstrate_crc(self, data):
        """Demonstrate CRC calculation and verification"""
        print("\n=== CRC Demonstration ===")
        print(f"Input Data: {data}")
        
        # Calculate CRC
        crc_value = crc_module.computeCRC(data)
        print(f"Calculated CRC: {crc_value}")
        
        # Verify CRC
        is_valid = crc_module.verifyCRC(data, crc_value)
        print(f"CRC Verification: {'Valid' if is_valid else 'Invalid'}")
        
        # Simulate error
        corrupted_data = crc_module.simulateError(data)
        is_valid_after_error = crc_module.verifyCRC(corrupted_data, crc_value)
        print(f"CRC Verification after error: {'Valid' if is_valid_after_error else 'Invalid'}")
        
        # Add to queue for animation
        self.crc_queue.put((data, crc_value, is_valid))
        
    def demonstrate_stop_and_wait(self, data):
        """Demonstrate Stop and Wait protocol"""
        print("\n=== Stop and Wait Protocol Demonstration ===")
        print(f"Sending data: {data}")
        seqNum = 0
        # Send packet using the function interface
        success, ack, new_seq = stop_and_wait_module.sendPacket(data, seqNum, self.sw_config)
        print(f"Packet sent successfully: {success}")
        print(f"Received ACK: {ack}")
        print(f"Next sequence number: {new_seq}")
        # Add to queue for animation
        self.stop_wait_queue.put((data, success, ack))
        
    def demonstrate_tcp_tahoe(self, data, num_packets=20):
        """Demonstrate TCP Tahoe protocol"""
        print("\n=== TCP Tahoe Protocol Demonstration ===")
        
        # Track metrics
        window_sizes = []
        rtts = []
        states = []
        throughputs = []
        start_time = time.time()
        bytes_sent = 0
        
        for i in range(num_packets):
            # Send packet
            success = self.tcp.sendData(data)
            if success:
                bytes_sent += len(data)
                
            # Simulate ACK reception
            time.sleep(random.uniform(0.05, 0.15))
            if random.random() < 0.8:  # 80% chance of successful ACK
                self.tcp.receiveAck(i * len(data))
            else:
                print("Simulating ACK loss")
                self.tcp.handleTimeout()
            
            # Get current stats
            stats = self.tcp.getStats()
            
            # Record metrics
            window_sizes.append(stats.currentWindowSize)
            rtts.append(stats.currentRTT)
            states.append(stats.currentState)
            
            # Calculate throughput
            current_time = time.time()
            throughput = bytes_sent / (current_time - start_time)
            throughputs.append(throughput)
            
            # Add to queue for animation
            self.tcp_queue.put((window_sizes[-1], rtts[-1], states[-1], throughputs[-1]))
            
            # Print progress
            print(f"\nPacket {i+1}/{num_packets}")
            print(f"Window Size: {stats.currentWindowSize}")
            print(f"RTT: {stats.currentRTT}ms")
            print(f"State: {stats.currentState}")
            print(f"Throughput: {throughput:.2f} bytes/s")
            
            self.tcp_window_history.append(stats.currentWindowSize)
            self.tcp_rtt_history.append(stats.currentRTT)
            self.tcp_state_history.append(stats.currentState)
            self.tcp_throughput_history.append(throughput)
            
    def update_plots(self, frame):
        """Update plots with new data"""
        # Update CRC plot
        if not self.crc_queue.empty():
            data, crc_value, is_valid = self.crc_queue.get()
            self.crc_ax.clear()
            self.crc_ax.bar(['Data', 'CRC'], [len(data), len(crc_value)])
            self.crc_ax.set_title('CRC Calculation')
            
        # Update Stop and Wait plot
        if not self.stop_wait_queue.empty():
            data, success, ack = self.stop_wait_queue.get()
            self.stop_wait_ax.clear()
            self.stop_wait_ax.plot([0, 1], [0, 1], 'b-', label='Packet')
            self.stop_wait_ax.plot([1, 2], [1, 0], 'g-', label='ACK')
            self.stop_wait_ax.set_title('Stop and Wait Protocol')
            self.stop_wait_ax.legend()
            
        # Update TCP plots
        if not self.tcp_queue.empty():
            window_size, rtt, state, throughput = self.tcp_queue.get()
            
            # Window size
            self.tcp_window_ax.clear()
            self.tcp_window_ax.plot(self.tcp_window_history, 'b-')
            self.tcp_window_ax.set_title('TCP Tahoe Window Size')
            
            # RTT
            self.tcp_rtt_ax.clear()
            self.tcp_rtt_ax.plot(self.tcp_rtt_history, 'r-')
            self.tcp_rtt_ax.set_title('TCP Tahoe RTT')
            
            # State
            self.tcp_state_ax.clear()
            self.tcp_state_ax.plot(self.tcp_state_history, 'g-')
            self.tcp_state_ax.set_title('TCP Tahoe State')
            
            # Throughput
            self.tcp_throughput_ax.clear()
            self.tcp_throughput_ax.plot(self.tcp_throughput_history, 'y-')
            self.tcp_throughput_ax.set_title('TCP Tahoe Throughput')
            
        plt.tight_layout()
        
    def run_demo(self, data):
        """Run the complete protocol demonstration"""
        # Start animation
        ani = FuncAnimation(self.fig, self.update_plots, interval=100)
        
        # Run protocols in separate threads
        crc_thread = threading.Thread(target=self.demonstrate_crc, args=(data,))
        stop_wait_thread = threading.Thread(target=self.demonstrate_stop_and_wait, args=(data,))
        tcp_thread = threading.Thread(target=self.demonstrate_tcp_tahoe, args=(data,))
        
        # Start threads
        crc_thread.start()
        stop_wait_thread.start()
        tcp_thread.start()
        
        # Show plot
        plt.show()
        
        # Wait for threads to complete
        crc_thread.join()
        stop_wait_thread.join()
        tcp_thread.join()

def main():
    # Create demo instance
    demo = ProtocolDemo()
    
    # Get user input
    print("Enter data to transmit (comma-separated numbers):")
    user_input = input()
    data = [int(x.strip()) for x in user_input.split(',')]
    
    # Run demonstration
    demo.run_demo(data)

if __name__ == "__main__":
    main() 