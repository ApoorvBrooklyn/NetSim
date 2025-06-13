#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "graph.h"

namespace py = pybind11;

PYBIND11_MODULE(graph_module, m) {
    py::class_<Graph>(m, "Graph")
        .def(py::init<int>())
        .def("addEdge", &Graph::addEdge)
        .def("primMST", &Graph::primMST);
} 