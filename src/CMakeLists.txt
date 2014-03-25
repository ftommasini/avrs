# src/CMakeLists.txt

# C++ source files
set(CXX_SOURCE_FILES 
	surface.cpp
        listener.cpp
        soundsource.cpp
        dxfreader.cpp
        configfilereader.cpp
       	tokenizer.cpp
       	convolver.cpp
        virtualenvironment.cpp
        headfilter.cpp
        trackersim.cpp
        rttools.cpp
        player.cpp
        input.cpp
        configuration.cpp
        fdn.cpp
        system.cpp     
        timer.cpp
        hrtfconvolver.cpp
	main.cpp
)

# C++ compiler
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -pipe -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__LINUX_ALSA__ -D__LITTLE_ENDIAN__ -D_REENTRANT")

# C compiler
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -pipe -Werror")

include_directories(
	/usr/realtime/include
	/usr/src/linux/include
)

link_directories(
	/usr/realtime/lib
)

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
add_library(trackerwiimote SHARED ${WIIMOTE_CXX_SOURCE_FILES} ${WIIMOTE_C_SOURCE_FILES})

# Executable file
add_executable(${PROJECT_NAME} ${CXX_SOURCE_FILES})

# Link executable file to libraries
target_link_libraries(${PROJECT_NAME} 
	${FFTW3_LIBRARY}
	${STK_LIBRARY}
	${ALSA_LIBRARIES}
	${M_LIBRARY}
	${CMAKE_THREAD_LIBS_INIT}
	${DXFLIB_LIBRARY}	
	${ARMADILLO_LIBRARY} 
	${ANN_LIBRARY}
	${Boost_LIBRARIES} 
	trackerwiimote
	${BLUETOOTH_LIBRARY}
	${CWIID_LIBRARY}
)

# Move to bin directory
#set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
