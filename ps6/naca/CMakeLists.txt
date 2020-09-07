add_library(naca4digit naca4digit.cpp naca4digit.h)
target_link_libraries(naca4digit ysclass polygonalmesh)
target_include_directories(naca4digit PUBLIC .)