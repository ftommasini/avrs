# src/tracker/CMakeLists.txt

add_subdirectory(sim)

if(WIIMOTE_SUPPORT)
    add_subdirectory(wiimote)  
endif()
