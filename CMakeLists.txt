cmake_minimum_required (VERSION 3.12)
project (INSITUAPI)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON) # sets -fPIC command line options for gcc

find_package (MPI REQUIRED)

add_library (InC2 STATIC src/message.cxx src/inC2.cxx src/inC2Comm.cxx )
set_target_properties (InC2 PROPERTIES PUBLIC_HEADER "src/message.h;src/inC2.h;src/inC2Comm.h")
target_link_libraries (InC2 PRIVATE MPI::MPI_C)

### Examples are old style InC2 and need to be updated. After, this block can be uncommented
#
#
#add_executable(multiplier examples/multiplier.cxx)
#add_executable(divider examples/divider.cxx)
#add_dependencies(multiplier InC2)
#add_dependencies(divider InC2)
#target_include_directories(multiplier PUBLIC src/)
#target_include_directories(divider PUBLIC src/)
#target_link_libraries (multiplier InC2 MPI::MPI_C)
#target_link_libraries (divider InC2 MPI::MPI_C)

#add_executable (c_example examples/c_example.cxx)
#add_dependencies(c_example InC2)
#target_include_directories(c_example PUBLIC src/)
#target_link_libraries(c_example InC2 MPI::MPI_C)
#
#
### End Examples

# When building with spack, sometimes the automatically set PYTHONHOME will cause a problem
unset (ENV{PYTHONHOME})

add_subdirectory (contrib/pybind11)
pybind11_add_module(PyInC2 bindings/python/PyInC2.cxx bindings/python/PyInC2Comm.cxx bindings/python/PyMessage.cxx)
target_link_libraries(PyInC2 PRIVATE InC2 MPI::MPI_C)

find_package(Python)

install (TARGETS InC2 
         LIBRARY DESTINATION lib
         ARCHIVE DESTINATION lib
         PUBLIC_HEADER DESTINATION include)
install (TARGETS PyInC2
         LIBRARY DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages)
