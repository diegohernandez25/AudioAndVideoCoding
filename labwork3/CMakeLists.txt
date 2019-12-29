cmake_minimum_required(VERSION 2.8) 
PROJECT (vmad_codec)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Ofast -Wall -Wextra")

link_directories( ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set( PLAYER src/player/player.cpp )
set( ENCODER2 src/codec2/encoder.cpp )

AUX_SOURCE_DIRECTORY( src/lib/bitstream/ BITSTREAM)
AUX_SOURCE_DIRECTORY( src/lib/golomb/ GOLOMB)
AUX_SOURCE_DIRECTORY( src/lib/predictor/ PREDICTOR)
AUX_SOURCE_DIRECTORY( src/lib/quantization/ QUANTIZATION)
AUX_SOURCE_DIRECTORY( src/lib/y4m/ Y4M)
AUX_SOURCE_DIRECTORY( src/lib/motion_compensator/ COMPENSATOR)

add_library( L_BITSTREAM STATIC ${BITSTREAM})
add_library( L_GOLOMB STATIC ${GOLOMB})
add_library( L_PREDICTOR STATIC ${PREDICTOR})
add_library( L_QUANTIZATION STATIC ${QUANTIZATION})
add_library( L_Y4M STATIC ${Y4M})
add_library( L_COMPENSATOR STATIC ${COMPENSATOR})

add_executable( player ${PLAYER} )
add_executable( encoder2 ${ENCODER2} )

target_link_libraries(L_Y4M
					${OpenCV_LIBS}
					)

target_link_libraries(L_COMPENSATOR
					${OpenCV_LIBS}
					)

target_link_libraries(player
					${OpenCV_LIBS}
					L_Y4M
					)

target_link_libraries(encoder2
					${OpenCV_LIBS}
					L_Y4M
					L_COMPENSATOR
					)
