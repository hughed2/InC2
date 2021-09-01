cmake_minimum_required (VERSION 3.0)
project (INSITUAPI)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (MPI REQUIRED)

add_library (inSituApi SHARED src/message.cxx src/inC2.cxx src/childProg.cxx )
#set_target_properties (inSituApi PROPERTIES PUBLIC_HEADER src/inC2.h)
target_link_libraries (inSituApi PRIVATE MPI::MPI_C)

add_executable(multiplier examples/multiplier.cxx)
add_executable(divider examples/divider.cxx)
add_dependencies(multiplier inSituApi)
add_dependencies(divider inSituApi)
target_include_directories(multiplier PUBLIC src/)
target_include_directories(divider PUBLIC src/)
target_link_libraries (multiplier inSituApi MPI::MPI_C)
target_link_libraries (divider inSituApi MPI::MPI_C)

add_executable (c_example examples/c_example.cxx)
add_dependencies(c_example inSituApi)
target_include_directories(c_example PUBLIC src/)
target_link_libraries(c_example inSituApi MPI::MPI_C)

find_package (pybind11 CONFIG)
pybind11_add_module(InC2 bindings/python/PyInC2.cxx bindings/python/PyChildProg.cxx bindings/python/PyMessage.cxx)
target_link_libraries(InC2 PRIVATE inSituApi MPI::MPI_C)

install (TARGETS inSituApi 
         LIBRARY DESTINATION lib
         PUBLIC_HEADER DESTINATION include)
