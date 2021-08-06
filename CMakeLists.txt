cmake_minimum_required (VERSION 3.0)
project (ADIOSAPI)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (MPI REQUIRED)
find_package (ADIOS2 REQUIRED)


add_library (adiosApi SHARED src/adiosApi.cxx)
set_target_properties (adiosApi PROPERTIES PUBLIC_HEADER src/adiosApi.h)
target_link_libraries (adiosApi PRIVATE adios2::cxx11_mpi MPI::MPI_C)

install (TARGETS adiosApi 
         LIBRARY DESTINATION lib
         PUBLIC_HEADER DESTINATION include)
