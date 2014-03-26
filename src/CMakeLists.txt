# src/CMakeLists.txt

add_subdirectory(utils)
add_subdirectory(tracker)

# C++ source files
set(CXX_SOURCE_FILES 
    surface.cpp
    listener.cpp
    soundsource.cpp
    dxfreader.cpp
    convolver.cpp
    virtualenvironment.cpp
    headfilter.cpp
    player.cpp
    input.cpp
    configuration.cpp
    fdn.cpp
    system.cpp
    hrtfconvolver.cpp
	main.cpp
)

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
	avrs_utils 
	avrs_trackersim
	avrs_trackerwiimote
	${BLUETOOTH_LIBRARY}
	${CWIID_LIBRARY}
)

# Move to bin directory
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
