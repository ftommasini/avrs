# src/tracker/sim/CMakeLists.txt

# Simulated tracker library

# C++ source files
set(SIM_CXX_SOURCE_FILES 
	trackersim.cpp
)

# Library file
add_library(avrs_trackersim SHARED ${SIM_CXX_SOURCE_FILES})

# Move to bin directory
#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)