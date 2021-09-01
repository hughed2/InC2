#include <pybind11/pybind11.h>

#include "../../src/message.h"

namespace py=pybind11;

void message_module(py::module &m)
{
   py::class_<Message>(m, "Message")
       .def(py::init<std::string>())
       .def(py::init<std::string, std::string>())
       .def("getText", &Message::getText)
       .def("getCommand", &Message::getCommand)
       .def("getPayload", &Message::getPayload)
   ;
}

