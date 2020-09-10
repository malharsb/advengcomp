add_executable(ps7 MACOSX_BUNDLE main.cpp ps7lib.cpp ps7lib.h)
target_link_libraries(ps7 fslazywindow geblkernel meshlattice glutil)
