cmake_minimum_required(VERSION 2.8) 
PROJECT (cav_1)

set(VIDHISTOGRAM
    src/wav_histogram.cpp    
)   

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable( vid_histogram ${VIDHISTOGRAM} )

target_link_libraries(vid_histogram sndfile)