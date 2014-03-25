# CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project(avrs)

# The version number
set(VERSION_MAJOR 0)
set(VERSION_MINOR 2)
set(VERSION_PATCH 1)

if (VERSION_PATCH MATCHES "0")
	set(VERSION_NUMBER "${VERSION_MAJOR}.${VERSION_MINOR}")
else()
	set(VERSION_NUMBER "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()

message(STATUS "Configuring ${PROJECT_NAME} ${VERSION_NUMBER}")

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

find_package(ALSA REQUIRED)

if (ALSA_FOUND)
	include_directories(${ALSA_INCLUDE_DIRS})
endif()

find_package(Threads)


#find_package(FFTW3 REQUIRED)

find_library(FFTW3_LIBRARY 
	NAMES fftw3f
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

find_library(M_LIBRARY m)

find_library(BLUETOOTH_LIBRARY bluetooth)

find_library(CWIID_LIBRARY cwiid)

# Optional libraries
find_package(OpenMP)

if (OPENMP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

add_subdirectory(src)
