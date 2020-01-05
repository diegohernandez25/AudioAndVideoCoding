cmake_minimum_required(VERSION 2.8) 
PROJECT (vmad_codec)

find_package(OpenCV REQUIRED)
find_package(Boost REQUIRED)

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -Ofast -Wall -Wextra")

link_directories( ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

set( PLAYER src/player/player.cpp )
set( ENCODER_1 src/codec1/enc_intra.cpp )
set( DECODER_1 src/codec1/dec_intra.cpp )
#set( ENCODER_2 src/codec2/encoder.cpp )
set( ENCODER_2 src/codec2/enc_hybrid.cpp )
set( DECODER_2 src/codec2/dec_hybrid.cpp )
set( ENCODER_3 src/codec3/enc_lossy.cpp )
set( DECODER_3 src/codec3/dec_lossy.cpp )

AUX_SOURCE_DIRECTORY( src/lib/bitstream/ BITSTREAM)
AUX_SOURCE_DIRECTORY( src/lib/golomb/ GOLOMB)
AUX_SOURCE_DIRECTORY( src/lib/predictor/ PREDICTOR)
AUX_SOURCE_DIRECTORY( src/lib/y4m/ Y4M)
AUX_SOURCE_DIRECTORY( src/lib/dct/rle/ RLE)
AUX_SOURCE_DIRECTORY( src/lib/dct/zigzag/ ZZ)
AUX_SOURCE_DIRECTORY( src/lib/dct/ DCT)
AUX_SOURCE_DIRECTORY( src/lib/args/ ARGS)
#AUX_SOURCE_DIRECTORY( src/parametizer/ PARAMETIZER)

add_library( L_ARGS STATIC ${ARGS})
add_library( L_BITSTREAM STATIC ${BITSTREAM})
add_library( L_GOLOMB STATIC ${GOLOMB})
add_library( L_PREDICTOR STATIC ${PREDICTOR})
add_library( L_Y4M STATIC ${Y4M})
add_library( L_RLE STATIC ${RLE})
add_library( L_ZZ STATIC ${ZZ})
add_library( L_DCT STATIC ${DCT})

add_executable( player ${PLAYER} )
add_executable( cod1 ${ENCODER_1} )
add_executable( cod2 ${ENCODER_2} )
add_executable( cod3 ${ENCODER_3} )
add_executable( dec1 ${DECODER_1} )
add_executable( dec2 ${DECODER_2} )
add_executable( dec3 ${DECODER_3} )

target_link_libraries(L_Y4M
					${OpenCV_LIBS}
					)

target_link_libraries(L_GOLOMB
					${OpenCV_LIBS}
					)

target_link_libraries(L_PREDICTOR
					${OpenCV_LIBS}
					)
target_link_libraries(L_ZZ
					${OpenCV_LIBS}
					)

target_link_libraries(L_DCT
					${OpenCV_LIBS}
					L_RLE
					L_ZZ
					)

target_link_libraries(player
					${OpenCV_LIBS}
					L_Y4M
					)

target_link_libraries(cod1
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					)

target_link_libraries(cod2
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					)

target_link_libraries(cod3
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					L_DCT
					)

target_link_libraries(dec1
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					)

target_link_libraries(dec2
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					)

target_link_libraries(dec3
					${OpenCV_LIBS}
					L_ARGS
					L_BITSTREAM
					L_GOLOMB
					L_PREDICTOR
					L_Y4M
					L_DCT
					)
