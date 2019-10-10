cmake_minimum_required(VERSION 2.8) 
PROJECT (cav_1)

FIND_PACKAGE( Boost REQUIRED )
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
set(AUDIOENTROPY
    src/audio_entropy.cpp    
)   

INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR}/include )
link_directories( ${CMAKE_BINARY_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
add_executable(audio_entropy ${AUDIOENTROPY} )

target_link_libraries(audio_entropy sndfile)