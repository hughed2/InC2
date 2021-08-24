cmake_minimum_required (VERSION 3.0)
project (INSITUAPI)

#set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package (MPI REQUIRED)
find_package (ADIOS2 REQUIRED)


add_library (inSituApi SHARED src/inSituApi.cxx)
set_target_properties (inSituApi PROPERTIES PUBLIC_HEADER src/inSituApi.h)
target_link_libraries (inSituApi PRIVATE adios2::cxx11_mpi MPI::MPI_C)

install (TARGETS inSituApi 
         LIBRARY DESTINATION lib
         PUBLIC_HEADER DESTINATION include)
