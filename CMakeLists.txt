cmake_minimum_required (VERSION 3.12)
project (INSITUAPI)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # sets -fPIC command line options for gcc

find_package (MPI REQUIRED COMPONENTS C)

add_library (InC2 STATIC src/message.cxx src/inC2.cxx src/inC2Comm.cxx )
set_target_properties (InC2 PROPERTIES PUBLIC_HEADER "src/message.h;src/inC2.h;src/inC2Comm.h;contrib/json/json.hpp")
include_directories("contrib/json")
target_link_libraries (InC2 PRIVATE MPI::MPI_C)

install (TARGETS InC2 
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include)

option(BUILD_PYTHON ON)
if(BUILD_PYTHON)
    # When building with spack, sometimes the automatically set PYTHONHOME will cause a problem
    unset (ENV{PYTHONHOME})

    add_subdirectory (contrib/pybind11)
    pybind11_add_module(PyInC2 bindings/python/PyInC2.cxx bindings/python/PyInC2Comm.cxx bindings/python/PyMessage.cxx)
    target_link_libraries(PyInC2 PRIVATE InC2 MPI::MPI_C)

    find_package(Python)

    install (TARGETS PyInC2
             LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
endif()