# Network Protocol Simulation

A comprehensive network protocol simulation project that demonstrates various networking concepts including CRC error detection, Stop-and-Wait protocol, and TCP Tahoe congestion control. The project is implemented using Python with C++ extensions for performance-critical components.

## Features

- **CRC Error Detection**: Implementation of Cyclic Redundancy Check (CRC) for error detection in data transmission
- **Stop-and-Wait Protocol**: A reliable data transfer protocol with sequence numbers and ACK handling
- **TCP Tahoe**: Implementation of TCP Tahoe congestion control algorithm with:
  - Slow Start
  - Congestion Avoidance
  - Fast Recovery
  - RTT estimation
  - Window scaling
  - Delayed ACKs

## Project Structure

```
network_sim/
├── cpp/                    # C++ implementation of core protocols
│   ├── include/           # Header files
│   ├── src/              # Source files
│   └── build/            # Build directory
├── python/               # Python interface and visualization
│   ├── protocol_demo.py  # Main demonstration script
│   ├── test_*.py        # Test scripts for each protocol
│   └── requirements.txt  # Python dependencies
└── README.md            # This file
```

## Prerequisites

- Python 3.9 or higher
- CMake 3.7 or higher
- C++17 compatible compiler
- pybind11

## Installation

1. Clone the repository:
   ```bash
   git clone <repository-url>
   cd network_sim
   ```

2. Create and activate a Python virtual environment:
   ```bash
   python -m venv python/myenv
   source python/myenv/bin/activate  # On Unix/macOS
   # or
   .\python\myenv\Scripts\activate  # On Windows
   ```

3. Install Python dependencies:
   ```bash
   pip install -r python/requirements.txt
   ```

4. Build C++ modules:
   ```bash
   cd cpp
   mkdir build && cd build
   cmake ..
   make
   ```

## Usage

### Running the Protocol Demonstration

The main demonstration script shows all three protocols in action with real-time visualization:

```bash
python python/protocol_demo.py
```

### Testing Individual Protocols

You can test each protocol separately using the provided test scripts:

```bash
# Test CRC
python python/test_crc.py

# Test Stop-and-Wait Protocol
python python/test_stop_and_wait.py

# Test TCP Tahoe
python python/test_tcp_tahoe.py
```

## Protocol Details

### CRC Error Detection
- Implements 32-bit CRC for error detection
- Supports error simulation and verification
- Visualizes CRC calculation process

### Stop-and-Wait Protocol
- Implements reliable data transfer with sequence numbers
- Handles packet loss and corruption
- Simulates network conditions (errors, losses, ACK loss)
- Configurable parameters for testing different scenarios

### TCP Tahoe
- Full implementation of TCP Tahoe congestion control
- Features:
  - Slow Start phase
  - Congestion Avoidance
  - Fast Recovery
  - RTT estimation using Jacobson's algorithm
  - Window scaling
  - Delayed ACK handling
- Real-time visualization of:
  - Window size
  - RTT
  - Protocol state
  - Throughput

## Visualization

The project includes real-time visualization of protocol behavior:
- Window size over time
- RTT measurements
- Protocol state transitions
- Throughput analysis

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- TCP Tahoe implementation based on RFC 2581
- CRC implementation using standard polynomial 0xEDB88320
- Stop-and-Wait protocol implementation following standard reliable data transfer principles
