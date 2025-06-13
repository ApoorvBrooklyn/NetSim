#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "crc.h"

namespace py = pybind11;

PYBIND11_MODULE(crc_module, m) {
    m.def("computeCRC", [](const std::vector<int>& data) {
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        return computeCRC(data_bytes);
    }, "Compute CRC for a data buffer");
    
    m.def("verifyCRC", [](const std::vector<int>& dataWithCRC) {
        std::vector<uint8_t> data_bytes(dataWithCRC.begin(), dataWithCRC.end());
        return verifyCRC(data_bytes);
    }, "Verify CRC for a data buffer with appended CRC");
    
    m.def("simulateError", [](const std::vector<int>& data) {
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        std::vector<uint8_t> result = simulateError(data_bytes);
        return std::vector<int>(result.begin(), result.end());
    }, "Simulate error by flipping a random bit in the data buffer");
} 