#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "stop_and_wait.h"

namespace py = pybind11;

PYBIND11_MODULE(stop_and_wait_module, m) {
    py::class_<ProtocolConfig>(m, "ProtocolConfig")
        .def(py::init<>())
        .def_readwrite("maxRetries", &ProtocolConfig::maxRetries)
        .def_readwrite("timeoutMs", &ProtocolConfig::timeoutMs)
        .def_readwrite("errorProbability", &ProtocolConfig::errorProbability)
        .def_readwrite("lossProbability", &ProtocolConfig::lossProbability)
        .def_readwrite("ackLossProbability", &ProtocolConfig::ackLossProbability);

    m.def("sendPacket", [](const std::vector<int>& data, int seqNum, ProtocolConfig& config) {
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        std::vector<uint8_t> ack;
        int seq = seqNum;
        bool success = sendPacket(data_bytes, ack, seq, config);
        return py::make_tuple(success, std::vector<int>(ack.begin(), ack.end()), seq);
    }, py::arg("data"), py::arg("seqNum"), py::arg("config"));

    m.def("receivePacket", [](std::vector<int>& data, int expectedSeqNum, ProtocolConfig& config) {
        std::vector<uint8_t> data_bytes(data.begin(), data.end());
        int seq = expectedSeqNum;
        bool valid = receivePacket(data_bytes, seq, config);
        data = std::vector<int>(data_bytes.begin(), data_bytes.end());
        return py::make_tuple(valid, data, seq);
    }, py::arg("data"), py::arg("expectedSeqNum"), py::arg("config"));
} 