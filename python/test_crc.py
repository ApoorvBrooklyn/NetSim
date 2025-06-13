import crc_module

# Sample data buffer
data = [0x01, 0x02, 0x03, 0x04, 0x05]

# Compute CRC
crc = crc_module.computeCRC(data)
print("Computed CRC:", hex(crc))

# Append CRC to data
dataWithCRC = data + [(crc >> 24) & 0xFF, (crc >> 16) & 0xFF, (crc >> 8) & 0xFF, crc & 0xFF]

# Verify CRC
isValid = crc_module.verifyCRC(dataWithCRC)
print("CRC Verification:", "Valid" if isValid else "Invalid")

# Simulate error by flipping a random bit
corruptedData = crc_module.simulateError(dataWithCRC)
isValidAfterError = crc_module.verifyCRC(corruptedData)
print("CRC Verification after error:", "Valid" if isValidAfterError else "Invalid") 