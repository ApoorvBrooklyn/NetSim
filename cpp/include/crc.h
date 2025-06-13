#pragma once
#include <vector>
#include <cstdint>

// Compute CRC for a data buffer
uint32_t computeCRC(const std::vector<uint8_t>& data);

// Verify CRC for a data buffer with appended CRC
bool verifyCRC(const std::vector<uint8_t>& dataWithCRC);

// Simulate error by flipping a random bit in the data buffer
std::vector<uint8_t> simulateError(const std::vector<uint8_t>& data); 