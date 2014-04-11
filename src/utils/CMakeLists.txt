# src/utils/CMakeLists.txt

# Utils library

# C++ source files
set(UTILS_CXX_SOURCE_FILES 
	configfilereader.cpp
	tokenizer.cpp
	rttools.cpp
	timerbase.cpp
	timercpu.cpp
	timerrtai.cpp
)

# Library file
add_library(avrs_utils SHARED ${UTILS_CXX_SOURCE_FILES})

# Move to bin directory
set_target_properties(avrs_utils PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
