cmake_minimum_required(VERSION 3.1)
project(kenlm-jni)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(JNI)
find_package(Boost REQUIRED)

include(ExternalProject)

set(kenlm_STATIC_LIBRARIES
    ${CMAKE_CURRENT_BINARY_DIR}/kenlm/src/kenlm-build/lib/libkenlm.a
    ${CMAKE_CURRENT_BINARY_DIR}/kenlm/src/kenlm-build/lib/libkenlm_util.a)
set(kenlm_INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}/kenlm/src/kenlm)
ExternalProject_Add(kenlm
    PREFIX kenlm
    GIT_REPOSITORY https://github.com/kpu/kenlm.git
    INSTALL_COMMAND ""
    CMAKE_CACHE_ARGS
        -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
        -DCMAKE_BUILD_TYPE:STRING=Release
)

add_library(kenlm_jni SHARED kenlm_KenlmJNI.cpp)
add_dependencies(kenlm_jni kenlm)
target_include_directories(kenlm_jni PUBLIC
    ${JNI_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${kenlm_INCLUDE_DIRS})
target_link_libraries(kenlm_jni ${kenlm_STATIC_LIBRARIES})

find_package(ZLIB)
if (ZLIB_FOUND)
    target_link_libraries(kenlm_jni ${ZLIB_LIBRARIES})
endif()
find_package(BZip2)
if (BZIP2_FOUND)
    target_link_libraries(kenlm_jni ${BZIP2_LIBRARIES})
endif()
find_package(LibLZMA)
if (LIBLZMA_FOUND)
    target_link_libraries(kenlm_jni ${LIBLZMA_LIBRARIES})
endif()

set(KENLM_MAX_ORDER 6 CACHE STRING "Maximum supported ngram order")
target_compile_definitions(kenlm_jni PUBLIC -DKENLM_MAX_ORDER=${KENLM_MAX_ORDER})

install(TARGETS kenlm_jni DESTINATION ${CMAKE_INSTALL_PREFIX})
