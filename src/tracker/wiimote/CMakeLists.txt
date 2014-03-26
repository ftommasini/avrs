# src/tracker/wiimote/CMakeLists.txt

# Wiimote-based tracker library

# C++ source files
set(WIIMOTE_CXX_SOURCE_FILES 
	posit.cpp
	trackerwiimote.cpp
	wiimote.cpp
)

# C source files
set(WIIMOTE_C_SOURCE_FILES
	svd.c
)

# Library file
add_library(avrs_trackerwiimote SHARED ${WIIMOTE_CXX_SOURCE_FILES} ${WIIMOTE_C_SOURCE_FILES})

# Move to bin directory
#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
