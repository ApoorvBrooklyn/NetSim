import stop_and_wait_module
import time

print("\n=== Testing Stop-and-Wait Protocol with Sequence Numbers and ACK Loss Simulation ===")

config = stop_and_wait_module.ProtocolConfig()
config.maxRetries = 5
config.timeoutMs = 1000
config.errorProbability = 0.2
config.lossProbability = 0.2
config.ackLossProbability = 0.3  # Increase to see ACK loss effect

print("Configuration:")
print(f"- Max Retries: {config.maxRetries}")
print(f"- Timeout: {config.timeoutMs}ms")
print(f"- Error Probability: {config.errorProbability}")
print(f"- Loss Probability: {config.lossProbability}")
print(f"- ACK Loss Probability: {config.ackLossProbability}")

# Initial sequence number
seqNum = 0

# Send a packet
print("\nSending packet...")
data = [1, 2, 3, 4, 5]
success, ack, nextSeq = stop_and_wait_module.sendPacket(data, seqNum, config)
print(f"Send Packet Success: {success}")
print(f"Received ACK: {ack}")
print(f"Next Sequence Number: {nextSeq}")

# Simulate receiving the packet
print("\nReceiving packet...")
received_data = [seqNum] + data  # Add seqNum as first byte
# Simulate CRC (for test, use the C++ CRC logic if needed)
# Here, we just call receivePacket to see the effect
valid, payload, nextExpectedSeq = stop_and_wait_module.receivePacket(received_data, seqNum, config)
print(f"Receive Packet Valid: {valid}")
print(f"Payload: {payload}")
print(f"Next Expected Sequence Number: {nextExpectedSeq}")

# Test multiple transmissions
print("\n=== Testing Multiple Transmissions with Sequence Numbers ===")
for i in range(3):
    print(f"\nTransmission {i+1}:")
    seqNum = i % 2
    success, ack, nextSeq = stop_and_wait_module.sendPacket(data, seqNum, config)
    print(f"Transmission {i+1} Success: {success}")
    print(f"ACK: {ack}")
    print(f"Next Sequence Number: {nextSeq}")
    time.sleep(0.5)  # Small delay between transmissions 