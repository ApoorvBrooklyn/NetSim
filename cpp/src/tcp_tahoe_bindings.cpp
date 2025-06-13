#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "tcp_tahoe.h"

namespace py = pybind11;

PYBIND11_MODULE(tcp_tahoe_module, m) {
    py::enum_<TCPState>(m, "TCPState")
        .value("SLOW_START", TCPState::SLOW_START)
        .value("CONGESTION_AVOIDANCE", TCPState::CONGESTION_AVOIDANCE)
        .value("FAST_RECOVERY", TCPState::FAST_RECOVERY)
        .export_values();

    py::class_<TCPConfig>(m, "TCPConfig")
        .def(py::init<>())
        .def_readwrite("initialWindowSize", &TCPConfig::initialWindowSize)
        .def_readwrite("maxWindowSize", &TCPConfig::maxWindowSize)
        .def_readwrite("mss", &TCPConfig::mss)
        .def_readwrite("initialRTT", &TCPConfig::initialRTT)
        .def_readwrite("timeout", &TCPConfig::timeout)
        .def_readwrite("lossProbability", &TCPConfig::lossProbability)
        .def_readwrite("ssthresh", &TCPConfig::ssthresh)
        .def_readwrite("windowScale", &TCPConfig::windowScale)
        .def_readwrite("delayedAckTimeout", &TCPConfig::delayedAckTimeout)
        .def_readwrite("maxDelayedAcks", &TCPConfig::maxDelayedAcks);

    py::class_<TCPStats>(m, "TCPStats")
        .def_readonly("totalPacketsSent", &TCPStats::totalPacketsSent)
        .def_readonly("totalPacketsLost", &TCPStats::totalPacketsLost)
        .def_readonly("totalRetransmissions", &TCPStats::totalRetransmissions)
        .def_readonly("currentWindowSize", &TCPStats::currentWindowSize)
        .def_readonly("currentRTT", &TCPStats::currentRTT)
        .def_readonly("srtt", &TCPStats::srtt)
        .def_readonly("rttvar", &TCPStats::rttvar)
        .def_readonly("rto", &TCPStats::rto)
        .def_readonly("totalBytesSent", &TCPStats::totalBytesSent)
        .def_readonly("totalBytesAcked", &TCPStats::totalBytesAcked)
        .def_readonly("totalDelayedAcks", &TCPStats::totalDelayedAcks)
        .def_readonly("totalFastRetransmits", &TCPStats::totalFastRetransmits)
        .def_readonly("currentState", &TCPStats::currentState);

    py::class_<TCPTahoe>(m, "TCPTahoe")
        .def(py::init<const TCPConfig&>(), py::arg("config") = TCPConfig())
        .def("sendData", [](TCPTahoe& self, const std::vector<int>& data) {
            std::vector<uint8_t> data_bytes(data.begin(), data.end());
            return self.sendData(data_bytes);
        })
        .def("receiveAck", &TCPTahoe::receiveAck)
        .def("handleTimeout", &TCPTahoe::handleTimeout)
        .def("handlePacketLoss", &TCPTahoe::handlePacketLoss)
        .def("getStats", &TCPTahoe::getStats)
        .def("resetStats", &TCPTahoe::resetStats)
        .def("getWindowSizeHistory", &TCPTahoe::getWindowSizeHistory);
} 