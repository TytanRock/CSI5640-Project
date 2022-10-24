cmake_minimum_required(VERSION 3.6)
project(picture_kmeans C)

set(SRCS main.c
         kmeans.c
         picture_decoder.c
         pixel_functions.c
         stopWatch.c
         types.c)

# Always optimize to 3 with all warnings to catch bugs
add_compile_options(-O3 -Wall)
if(ACCELERATION_TYPE STREQUAL "NO_ACCEL")
    set(CMAKE_C_COMPILER "gcc")
    set(OUTPUT_NAME "no_accel_${PROJECT_NAME}")
elseif(ACCELERATION_TYPE STREQUAL "THREADED")
    set(CMAKE_C_COMPILER "$ENV{HOME}/pgccSDK/Linux_x86_64/22.7/compilers/bin/pgcc")
    set(OUTPUT_NAME "threaded_${PROJECT_NAME}")
    add_compile_options(-acc -ta=multicore -Minfo=accel)
elseif(ACCELERATION_TYPE STREQUAL "GPU")
    set(CMAKE_C_COMPILER "$ENV{HOME}/pgccSDK/Linux_x86_64/22.7/compilers/bin/pgcc")
    set(OUTPUT_NAME "gpu_accel_${PROJECT_NAME}")
    add_compile_options(-acc -ta=tesla:cc35,nofma -Minfo=accel)
else()
    message(FATAL_ERROR "Acceleration type not defined, pass either into -DACCELERATION_TYPE:STRING=<NO_ACCEL, THREADED, GPU>")
endif()

add_executable(${OUTPUT_NAME} ${SRCS})

if(ACCELERATION_TYPE STREQUAL "NO_ACCEL")
    # All is good, no need to add additional linker flags
elseif(ACCELERATION_TYPE STREQUAL "THREADED")
    # Need the additional linker flags since cmake doesn't automatically bring them over from the compile step
    set_target_properties(${OUTPUT_NAME} PROPERTIES LINK_FLAGS "-acc -ta=multicore -Minfo=accel")
elseif(ACCELERATION_TYPE STREQUAL "GPU")
    set_target_properties(${OUTPUT_NAME} PROPERTIES LINK_FLAGS "-acc -ta=tesla:cc35,nofma -Minfo=accel")
endif()

