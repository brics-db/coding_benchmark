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
    PROJECT(CodingBenchmark VERSION 0.6 LANGUAGES CXX)
ELSE()
    PROJECT(CodingBenchmark LANGUAGES CXX)
ENDIF()

ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-pedantic)
ADD_DEFINITIONS(-march=native)
ADD_DEFINITIONS(-Wno-ignored-attributes)

MACRO( add_algorithm arg_name arg_source_files)
    STRING(REPLACE " " ";" TMP_SOURCE_FILES "${arg_source_files}")
    FOREACH(f ${TMP_SOURCE_FILES})
        LIST(APPEND SOURCE_FILES "${f}")
    ENDFOREACH(f TMP_SOURCE_FILES)

    ADD_EXECUTABLE(${arg_name} ${SOURCE_FILES})

    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD 14)
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD_REQUIRED ON)
#	TARGET_COMPILE_OPTIONS(${arg_name} PRIVATE "-O2")

    MESSAGE(STATUS "Add executable ${arg_name}")
ENDMACRO( add_algorithm )

MACRO(append_target_property arg_target arg_property arg_setting)
	GET_TARGET_PROPERTY(TEMP ${arg_target} ${arg_property})
	IF(TEMP STREQUAL "TEMP-NOTFOUND")
		SET(TEMP "") # set to empty string
	ELSE()
		SET(TEMP "${TEMP} ") # a space to cleanly separate from existing content
	ENDIF()
	# append our values
	SET(TEMP "${TEMP}${arg_setting}")
	SET_TARGET_PROPERTIES(${arg_target} PROPERTIES ${arg_property} ${TEMP})
ENDMACRO(append_target_property)

add_algorithm(benchmark "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")

#add_algorithm(benchmark_novec "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")
#TARGET_COMPILE_OPTIONS(benchmark_novec PRIVATE "-fno-tree-vectorize")

add_algorithm(benchmarkOMP "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")
TARGET_COMPILE_DEFINITIONS(benchmarkOMP PRIVATE "OMP")
append_target_property(benchmarkOMP "COMPILE_FLAGS" "-fopenmp")
append_target_property(benchmarkOMP "LINK_FLAGS" "-fopenmp")

add_algorithm(benchmarkOMP2 "Main.cpp Test.cpp Util/ErrorInfo.cpp Util/TestInfo.cpp Util/Stopwatch.cpp Util/CPU.cpp")
TARGET_COMPILE_DEFINITIONS(benchmarkOMP2 PRIVATE "OMP" "OMPNUMTHREADS=2")
append_target_property(benchmarkOMP2 "COMPILE_FLAGS" "-fopenmp")
append_target_property(benchmarkOMP2 "LINK_FLAGS" "-fopenmp")

