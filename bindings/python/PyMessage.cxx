#include <pybind11/pybind11.h>

#include "../../src/message.h"

namespace py=pybind11;

void message_module(py::module &m)
{
   py::class_<Message>(m, "Message")
       .def(py::init<std::string>())
       .def("getCommand", &Message::getCommand)
       .def("getString", &Message::getString)
       .def("addIntParameter", &Message::addIntParameter)
       .def("addFloatParameter", &Message::addFloatParameter)
       .def("addStringParameter", &Message::addStringParameter)
       .def("removeParameter", &Message::removeParameter)
       .def("getIntParameter", &Message::getIntParameter)
       .def("getFloatParameter", &Message::getFloatParameter)
       .def("getStringParameter", &Message::getStringParameter)
   ;
}

