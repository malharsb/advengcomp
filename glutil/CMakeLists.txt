add_library(glutil glutil.cpp glutil.h)
target_link_libraries(glutil ysclass)
target_include_directories(glutil PUBLIC .)
