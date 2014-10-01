# CMakeLists.txt

cmake_minimum_required(VERSION 2.8)
project(avrs)

# The version number
set(VERSION_MAJOR 0)
set(VERSION_MINOR 5)
set(VERSION_PATCH 0)

# Configuration options
set(WIIMOTE_SUPPORT OFF CACHE BOOL 
    "Build avrs with wiimote-based tracker library")

if(VERSION_PATCH MATCHES "0")
	set(VERSION_NUMBER "${VERSION_MAJOR}.${VERSION_MINOR}")
else()
	set(VERSION_NUMBER "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
endif()

message(STATUS "Configuring ${PROJECT_NAME} ${VERSION_NUMBER}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file(
  "${PROJECT_SOURCE_DIR}/include/version.hpp.in"
  "${PROJECT_SOURCE_DIR}/include/version.hpp"
)

# Required libraries

# RTAI-LXRT
set(RTAI_SEARCH_PATH
    /usr
    /usr/local
    /usr/realtime
)
	
find_path(RTAI_DIR
    include/rtai.h 
    ${RTAI_SEARCH_PATH}
)
	
if(RTAI_DIR)
    set(RTAI_INCLUDE_DIR ${RTAI_DIR}/include)
    find_library(RTAI_LIBRARY lxrt ${RTAI_DIR}/lib)
endif()

if(${RTAI_LIBRARY} STREQUAL "RTAI_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found lxrt library")
else()	
	message(STATUS "Found lxrt library: " ${RTAI_LIBRARY})
endif()

# Armadillo
find_package(Armadillo REQUIRED)

# Boost
find_package(Boost COMPONENTS program_options system filesystem REQUIRED)

if(Boost_FOUND)
	include_directories(${Boost_INCLUDE_DIRS})
	add_definitions(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
endif()

# ALSA
find_package(ALSA REQUIRED)

if(ALSA_FOUND)
	include_directories(${ALSA_INCLUDE_DIRS})
endif()

# pthreads
find_package(Threads)

# FFTW3 (float)
find_library(FFTW3_LIBRARY fftw3f)

if(${FFTW3_LIBRARY} STREQUAL "FFTW3_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found fftw3 library")
else()	
	message(STATUS "Found fftw3 library: " ${FFTW3_LIBRARY})
endif()

# STK
find_library(STK_LIBRARY stk)

if(${STK_LIBRARY} STREQUAL "STK_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found STK library")
else()	
	message(STATUS "Found STK library: " ${STK_LIBRARY})
endif()

# dxflib
find_library(DXFLIB_LIBRARY dxflib)

if(${DXFLIB_LIBRARY} STREQUAL "DXFLIB_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found dxflib library")
else()	
	message(STATUS "Found dxflib library: " ${DXFLIB_LIBRARY})
endif()

# ANN
find_library(ANN_LIBRARY ANN)

if(${ANN_LIBRARY} STREQUAL "ANN_LIBRARY-NOTFOUND")
	message(FATAL_ERROR "Not found ANN library")
else()	
	message(STATUS "Found ANN library: " ${ANN_LIBRARY})
endif()

# Math
find_library(M_LIBRARY m)

if(WIIMOTE_SUPPORT)
    # Bluetooth
    find_library(BLUETOOTH_LIBRARY bluetooth)
    # Cwiid
    find_library(CWIID_LIBRARY cwiid)  
endif()

# Optional libraries

# OpenMP
find_package(OpenMP)

# Build type
if(NOT CMAKE_BUILD_TYPE)  # Debug by default
    set(CMAKE_BUILD_TYPE Debug CACHE STRING
        "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel"
        FORCE)
endif()

message(STATUS "Build type ${CMAKE_BUILD_TYPE}")

# Set flags for Debug (overwrite flags)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O2 -pipe")  
set(CMAKE_C_FLAGS_DEBUG "-g -O2 -pipe")

# Set flags for Release (overwrite flags)
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -pipe -DNDEBUG")  
set(CMAKE_C_FLAGS_RELEASE "-O2 -pipe -DNDEBUG")

# Set flags on C++ compiler (append flags)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__ -D_REENTRANT")

if(OPENMP_FOUND)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

# Set flags on C compiler (append flags)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror")

include_directories(
	include
	${RTAI_DIR}/include
	/usr/src/linux/include
)

link_directories(
	${RTAI_DIR}/lib
)

add_subdirectory(src)
