#include <pybind11/pybind11.h>

#include "../../src/inC2.h"

namespace py=pybind11;

void childProg_module(py::module &);
void message_module(py::module &);

PYBIND11_MODULE(InC2, m)
{
   py::class_<InC2>(m, "InC2")
       .def(py::init<>())
       .def("spawnMPIChild", &InC2::spawnMPIChild)
       .def("sendParentMessage", &InC2::sendParentMessage)
       .def("receiveParentMessage", &InC2::receiveParentMessage)
   ;

  childProg_module(m);
  message_module(m);
}

