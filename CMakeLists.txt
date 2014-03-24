# CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project(avrs)

# The version number
set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 1)

message(STATUS "Configuring ${PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}")

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file(
#  "${PROJECT_SOURCE_DIR}/include/version.hpp.in"
#  "${PROJECT_SOURCE_DIR}/include/version.hpp"
#)

# Required libraries
find_package(Armadillo REQUIRED)
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)

if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()

include_directories(${Boost_INCLUDE_DIRS})

#find_package(FFTW3 REQUIRED)

find_library(FFTW3_LIBRARY 
	NAMES fftw3
)

if (${FFTW3_LIBRARY} STREQUAL "FFTW3_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found fftw3 library")
else()	
	message(STATUS "Found fftw3 library: " ${FFTW3_LIBRARY})
endif()

find_library(DXFLIB_LIBRARY 
	NAMES dxflib
)

find_library(STK_LIBRARY 
	NAMES stk
)

if (${STK_LIBRARY} STREQUAL "STK_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found STK library")
else()	
	message(STATUS "Found STK library: " ${STK_LIBRARY})
endif()

if (${DXFLIB_LIBRARY} STREQUAL "DXFLIB_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found dxflib library")
else()	
	message(STATUS "Found dxflib library: " ${DXFLIB_LIBRARY})
endif()

find_library(ANN_LIBRARY 
	NAMES ANN
)

if (${ANN_LIBRARY} STREQUAL "ANN_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found ANN library")
else()	
	message(STATUS "Found ANN library: " ${ANN_LIBRARY})
endif()

#LIBS = -lfftw3f -lc -lm -lasound -lpthread 
# -lbluetooth -lcwiid

# Optional libraries
find_package(OpenMP)

if (OPENMP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

include_directories(include)
add_subdirectory(src)
