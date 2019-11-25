cmake_minimum_required(VERSION 2.8) 
PROJECT (madv_codec)

find_package(OpenCV REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -Wextra")

link_directories( ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set( PLAYER src/player/player.cpp )

AUX_SOURCE_DIRECTORY( src/lib/bitstream/ BITSTREAM)
AUX_SOURCE_DIRECTORY( src/lib/golomb/ GOLOMB)
AUX_SOURCE_DIRECTORY( src/lib/predictor/ PREDICTOR)
AUX_SOURCE_DIRECTORY( src/lib/quantization/ QUANTIZATION)
AUX_SOURCE_DIRECTORY( src/lib/y4m/ Y4M)
#AUX_SOURCE_DIRECTORY( src/parametizer/ PARAMETIZER)

add_library( L_BITSTREAM STATIC ${BITSTREAM})
add_library( L_GOLOMB STATIC ${GOLOMB})
add_library( L_PREDICTOR STATIC ${PREDICTOR})
add_library( L_QUANTIZATION STATIC ${QUANTIZATION})
add_library( L_Y4M STATIC ${Y4M})

add_executable( player ${PLAYER} )

target_link_libraries(L_Y4M
					${OpenCV_LIBS}
					)

target_link_libraries(player
					${OpenCV_LIBS}
					L_Y4M
					)
