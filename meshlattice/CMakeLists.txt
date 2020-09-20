add_library(meshlattice meshlattice.cpp meshlattice.h lattice.h)
target_link_libraries(meshlattice ysclass geblkernel)
target_include_directories(meshlattice PUBLIC .)
