cmake_minimum_required(VERSION 2.8) 
PROJECT (cav_1)
find_package(OpenCV REQUIRED )
set( HIST
    src/main.cpp    
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
link_directories( ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable( hist ${HIST} )

target_link_libraries( hist ${OpenCV_LIBS} )

