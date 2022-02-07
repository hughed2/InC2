#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../src/inC2Comm.h"

namespace py=pybind11;

void inC2Comm_module(py::module &m)
{
   py::class_<InC2Comm>(m, "InC2Comm")
       .def("sendMessage", &InC2Comm::sendMessage)
       .def("checkForMessage", &InC2Comm::checkForMessage)
       .def("stop", &InC2Comm::stop)
       .def("report", &InC2Comm::report)
       .def("input", &InC2Comm::input)
       .def("sendDoubles", &InC2Comm::sendDoubles)
       .def("sendDoublesToAll", &InC2Comm::sendDoublesToAll)
       .def("sendInts", &InC2Comm::sendInts)
       .def("sendIntsToAll", &InC2Comm::sendIntsToAll)
       .def("waitForAsync", &InC2Comm::waitForAsync)
       .def("receiveDoubles", &InC2Comm::receiveDoubles)
       .def("receiveDoublesFromAll", &InC2Comm::receiveDoublesFromAll)
       .def("receiveInts", &InC2Comm::receiveInts)
       .def("receiveIntsFromAll", &InC2Comm::receiveIntsFromAll)
   ;
}

