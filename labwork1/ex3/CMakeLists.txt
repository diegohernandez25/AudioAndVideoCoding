cmake_minimum_required(VERSION 2.8) 
PROJECT (cav_1)
find_package(OpenCV REQUIRED )
set( VIEWER
    src/img_vid_viewer.cpp    
)

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( viewer ${VIEWER} )

target_link_libraries( viewer ${OpenCV_LIBS} )

