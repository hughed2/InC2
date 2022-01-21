#include <pybind11/pybind11.h>

#include "../../src/inC2.h"

namespace py=pybind11;

void childProg_module(py::module &);
void message_module(py::module &);

PYBIND11_MODULE(PyInC2, m)
{
   py::class_<InC2>(m, "PyInC2")
       .def(py::init<>())
       .def("spawnMPIChild", &InC2::spawnMPIChild)
       .def("sendParentMessage", &InC2::sendParentMessage)
       .def("receiveParentMessage", &InC2::receiveParentMessage)
       .def("sendDoubles", &InC2::sendDoubles)
       .def("sendInts", &InC2::sendInts)
       .def("waitForAsync", &InC2::waitForAsync)
   ;

  childProg_module(m);
  message_module(m);
}

