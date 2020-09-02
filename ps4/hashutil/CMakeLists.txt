add_library(hashutil
	hash.cpp
	hashbase.h
	hashset.h
	hashtable.h)
target_include_directories(hashutil PUBLIC .)
