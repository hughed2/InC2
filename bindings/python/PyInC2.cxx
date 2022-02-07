#include <pybind11/pybind11.h>

#include "../../src/inC2.h"

namespace py=pybind11;

void inC2Comm_module(py::module &);
void message_module(py::module &);

PYBIND11_MODULE(PyInC2, m)
{
   py::class_<InC2>(m, "PyInC2")
       .def(py::init<>())
       .def("spawnChild", &InC2::spawnChild)
       .def("getParent", &InC2::getParent)
   ;

  inC2Comm_module(m);
  message_module(m);
}

