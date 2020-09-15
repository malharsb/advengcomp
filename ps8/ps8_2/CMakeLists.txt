add_library(astar astar.h astar.cpp)
target_include_directories(astar PUBLIC .)
target_link_libraries(astar geblkernel)

add_executable(ps8_2 MACOSX_BUNDLE main.cpp glutil.cpp glutil.h)
target_link_libraries(ps8_2 fslazywindow geblkernel astar)

