# Copyright 2016 Till Kolditz, Stefan de Bruijn
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

CMAKE_MINIMUM_REQUIRED(VERSION 3.1 FATAL_ERROR)

SET(CMAKE_C_COMPILER_INIT gcc-6)
SET(CMAKE_CXX_COMPILER_INIT g++-6)

IF(POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
    PROJECT(CodingBenchmark VERSION 0.5 LANGUAGES CXX)
ELSE()
    PROJECT(CodingBenchmark LANGUAGES CXX)
ENDIF()

ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-pedantic)
ADD_DEFINITIONS(-march=native)
ADD_DEFINITIONS(-Wno-ignored-attributes)

MACRO( add_vectorized arg_name arg_source_files)
    STRING(REPLACE " " ";" TMP_SOURCE_FILES "${arg_source_files}")
    FOREACH(f ${TMP_SOURCE_FILES})
        LIST(APPEND SOURCE_FILES "${f}")
    ENDFOREACH(f TMP_SOURCE_FILES)

    ADD_EXECUTABLE(${arg_name} ${SOURCE_FILES})

    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD 14)
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD_REQUIRED ON)
#	TARGET_COMPILE_OPTIONS(${arg_name} PRIVATE "-O2")

    MESSAGE(STATUS "Add vectorized executable ${arg_name}")
ENDMACRO( add_vectorized )

MACRO( add_nonvectorized arg_name arg_source_files)

    STRING(REPLACE " " ";" TMP_SOURCE_FILES "${arg_source_files}")
    FOREACH(f ${TMP_SOURCE_FILES})
        LIST(APPEND SOURCE_FILES "${f}")
    ENDFOREACH(f TMP_SOURCE_FILES)

    ADD_EXECUTABLE(${arg_name} ${SOURCE_FILES})

    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD 14)
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_COMPILE_OPTIONS(${arg_name} PRIVATE "-fno-tree-vectorize")

    MESSAGE(STATUS "Add nonvectorized executable ${arg_name}")
ENDMACRO( add_nonvectorized )

add_vectorized(benchmark "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")
add_nonvectorized(benchmark_novec "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")

