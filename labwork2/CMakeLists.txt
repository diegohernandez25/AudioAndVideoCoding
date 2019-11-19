cmake_minimum_required(VERSION 2.8) 
PROJECT (mad_codec)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast -Wall -Wextra")

set( MAD_CODEC src/mad_codec.cpp )
AUX_SOURCE_DIRECTORY(src/lib/bitstream/ BITSTREAM)
AUX_SOURCE_DIRECTORY( src/lib/golomb/ GOLOMB)
AUX_SOURCE_DIRECTORY( src/lib/predictor/ PREDICTOR)
AUX_SOURCE_DIRECTORY( src/lib/quantization/ QUANTIZATION)
AUX_SOURCE_DIRECTORY( src/lib/wav/ WAV)
AUX_SOURCE_DIRECTORY( src/lossless/ LOSSLESS)
AUX_SOURCE_DIRECTORY( src/lossy/ LOSSY)
AUX_SOURCE_DIRECTORY( src/parametizer/ PARAMETIZER)

add_library( L_BITSTREAM STATIC ${BITSTREAM})
add_library( L_GOLOMB STATIC ${GOLOMB})
add_library( L_PREDICTOR STATIC ${PREDICTOR})
add_library( L_QUANTIZATION STATIC ${QUANTIZATION})
add_library( L_WAV STATIC ${WAV})
add_library( L_LOSSLESS STATIC ${LOSSLESS})
add_library( L_LOSSY STATIC ${LOSSY})
add_library( L_PARAMETIZER STATIC ${PARAMETIZER})
add_executable( mad_codec ${MAD_CODEC} )

link_directories( ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

target_link_libraries(L_LOSSLESS
					L_BITSTREAM 
					L_GOLOMB 
					L_PREDICTOR 
					L_QUANTIZATION
					L_WAV
					L_PARAMETIZER
					)
target_link_libraries(L_LOSSY
					L_LOSSLESS
					)

target_link_libraries(mad_codec
					L_LOSSY
					)
