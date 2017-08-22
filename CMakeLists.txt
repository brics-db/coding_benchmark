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

IF(POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
    PROJECT(CodingBenchmark VERSION 0.6 LANGUAGES CXX)
ELSE()
    PROJECT(CodingBenchmark LANGUAGES CXX)
ENDIF()

ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-pedantic)
ADD_DEFINITIONS(-march=native)
#ADD_DEFINITIONS(-mavx2)
ADD_DEFINITIONS(-Wno-ignored-attributes)
ADD_DEFINITIONS(-Weffc++)

MACRO( add_algorithm arg_name arg_source_files)
	STRING(REPLACE " " ";" src_files "${arg_source_files}")
    ADD_EXECUTABLE(${arg_name} ${src_files})
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_INCLUDE_DIRECTORIES(${arg_name} PUBLIC src)
    MESSAGE("Added executable ${arg_name}")
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

add_algorithm(benchmark "src/Main.cpp src/Test.cpp src/Output.cpp \
src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Util/SIMD.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_avx2.cpp \
src/XOR/XOR_base.cpp src/XOR/XOR_scalar.cpp src/XOR/XOR_sse42.cpp src/XOR/XOR_avx2.cpp \
src/AN/AN_scalar.cpp src/AN/AN_sse42.cpp src/AN/AN_avx2.cpp")
add_algorithm(AN_Datalength_Comparison "src/AN_Datalength_Comparison.cpp src/Test.cpp src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Output.cpp \
src/AN/AN_scalar.cpp")

add_algorithm(benchmark_novec "src/Main.cpp src/Test.cpp src/Output.cpp \
src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Util/SIMD.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_avx2.cpp \
src/XOR/XOR_base.cpp src/XOR/XOR_scalar.cpp src/XOR/XOR_sse42.cpp src/XOR/XOR_avx2.cpp \
src/AN/AN_scalar.cpp src/AN/AN_sse42.cpp src/AN/AN_avx2.cpp")
TARGET_COMPILE_OPTIONS(benchmark_novec PRIVATE "-fno-tree-vectorize")

FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
	add_algorithm(benchmarkOMP2 "src/Main.cpp src/Test.cpp src/Output.cpp \
src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Util/SIMD.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_avx2.cpp \
src/XOR/XOR_base.cpp src/XOR/XOR_scalar.cpp src/XOR/XOR_sse42.cpp src/XOR/XOR_avx2.cpp \
src/AN/AN_scalar.cpp src/AN/AN_sse42.cpp src/AN/AN_avx2.cpp")
	TARGET_COMPILE_DEFINITIONS(benchmarkOMP2 PRIVATE "OMP" "OMPNUMTHREADS=2")
	append_target_property(benchmarkOMP2 "COMPILE_FLAGS" "${OpenMP_CXX_FLAGS}")
	append_target_property(benchmarkOMP2 "LINK_FLAGS" "${OpenMP_CXX_FLAGS}")

	add_algorithm(benchmarkOMP "src/Main.cpp src/Test.cpp src/Output.cpp \
src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Util/SIMD.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_avx2.cpp \
src/XOR/XOR_base.cpp src/XOR/XOR_scalar.cpp src/XOR/XOR_sse42.cpp src/XOR/XOR_avx2.cpp \
src/AN/AN_scalar.cpp src/AN/AN_sse42.cpp src/AN/AN_avx2.cpp")
	TARGET_COMPILE_DEFINITIONS(benchmarkOMP PRIVATE "OMP")
	append_target_property(benchmarkOMP "COMPILE_FLAGS" "${OpenMP_CXX_FLAGS}")
	append_target_property(benchmarkOMP "LINK_FLAGS" "${OpenMP_CXX_FLAGS}")
ENDIF(OPENMP_FOUND)

add_algorithm(TestModuloInverseComputation "src/TestModuloInverseComputation.cpp src/Util/Stopwatch.cpp")
add_algorithm(TestModuloInverseComputation2 "src/TestModuloInverseComputation2.cpp src/Util/Stopwatch.cpp")
add_algorithm(TestComputeHamming "src/TestComputeHamming.cpp src/Test.cpp src/Output.cpp src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/Util/SIMD.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_compute.cpp")

