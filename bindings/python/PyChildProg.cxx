#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "../../src/childProg.h"

namespace py=pybind11;

void childProg_module(py::module &m)
{
   py::class_<ChildProg>(m, "ChildProg")
       .def(py::init<std::string, bool>())
       .def("sendMessage", &ChildProg::sendMessage)
       .def("receiveMessage", &ChildProg::receiveMessage)
       .def("stop", &ChildProg::stop)
       .def("report", &ChildProg::report)
       .def("input", &ChildProg::input)
       .def("receiveDoubles", &ChildProg::receiveDoubles)
       .def("receiveDoublesFromAll", &ChildProg::receiveDoublesFromAll)
       .def("receiveInts", &ChildProg::receiveInts)
       .def("receiveIntsFromAll", &ChildProg::receiveIntsFromAll)
   ;
}

