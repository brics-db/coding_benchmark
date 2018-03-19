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

# The high CMake version is required due to dependency on C++17 features!
CMAKE_MINIMUM_REQUIRED(VERSION 3.8 FATAL_ERROR)

IF(POLICY CMP0048)
    CMAKE_POLICY(SET CMP0048 NEW)
    PROJECT(CodingBenchmark VERSION 1.0 LANGUAGES CXX)
ELSE()
    PROJECT(CodingBenchmark LANGUAGES CXX)
ENDIF()

SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})
SET(PROJECT_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)
MESSAGE(STATUS "PROJECT_INCLUDE_DIR = ${PROJECT_INCLUDE_DIR}")
INCLUDE_DIRECTORIES("${PROJECT_INCLUDE_DIR}")

ADD_DEFINITIONS(-Wall)
ADD_DEFINITIONS(-pedantic)
ADD_DEFINITIONS(-march=native)
#ADD_DEFINITIONS(-mavx2)
#ADD_DEFINITIONS(-march=knl)
ADD_DEFINITIONS(-Weffc++)
ADD_DEFINITIONS(-Wno-ignored-attributes)

MACRO( add_static_lib arg_name arg_source_files arg_dep_libs )
    SET(LIBNAME "${arg_name}")
    STRING(REPLACE " " ";" SOURCE_FILES "${arg_source_files}")
    MESSAGE(STATUS "Add static library ${LIBNAME}")
    ADD_LIBRARY(${LIBNAME} STATIC ${SOURCE_FILES})
    SET_PROPERTY(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET ${LIBNAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_INCLUDE_DIRECTORIES(${arg_name} PUBLIC src)
    TARGET_LINK_LIBRARIES(${LIBNAME} pthread "${arg_dep_libs}")
ENDMACRO( add_static_lib )

MACRO( add_shared_lib arg_name arg_source_files arg_dep_libs )
    SET(LIBNAME "${arg_name}")

    STRING(REPLACE " " ";" SOURCE_FILES "${arg_source_files}")

    MESSAGE(STATUS "Add shared library ${LIBNAME}")
    ADD_LIBRARY(${LIBNAME} SHARED ${SOURCE_FILES})
    SET_PROPERTY(TARGET ${LIBNAME} PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET ${LIBNAME} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_INCLUDE_DIRECTORIES(${arg_name} PUBLIC src)
    TARGET_LINK_LIBRARIES(${LIBNAME} pthread "${arg_dep_libs}")
ENDMACRO( add_shared_lib )

MACRO( add_algorithm arg_name arg_source_files )
	STRING(REPLACE " " ";" src_files "${arg_source_files}")
    ADD_EXECUTABLE(${arg_name} ${src_files})
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD 17)
    SET_PROPERTY(TARGET ${arg_name} PROPERTY CXX_STANDARD_REQUIRED ON)
    TARGET_INCLUDE_DIRECTORIES(${arg_name} PUBLIC src)
    SET(extra_macro_args ${ARGN})
    # Did we get any optional args?
    LIST(LENGTH extra_macro_args num_extra_args)
    IF(${num_extra_args} GREATER 0)
    	LIST(GET extra_macro_args 0 optional_arg)
    	TARGET_LINK_LIBRARIES(${arg_name} ${optional_arg})
    ENDIF(${num_extra_args} GREATER 0)
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

add_static_lib(benchbase "src/Util/Output.cpp src/Util/ErrorInfo.cpp src/Util/TestInfo.cpp src/Util/Stopwatch.cpp src/Util/CPU.cpp src/SIMD/SSE.cpp src/SIMD/AVX2.cpp src/SIMD/AVX512.cpp \
src/Hamming/Hamming_compute.cpp \
src/Hamming/Hamming_scalar.cpp src/Hamming/Hamming_scalar_16.cpp src/Hamming/Hamming_scalar_32.cpp \
src/Hamming/Hamming_sse42.cpp src/Hamming/Hamming_sse42_16.cpp src/Hamming/Hamming_sse42_32.cpp \
src/Hamming/Hamming_avx2.cpp src/Hamming/Hamming_avx2_16.cpp src/Hamming/Hamming_avx2_32.cpp \
src/Hamming/Hamming_avx512.cpp src/Hamming/Hamming_avx512_16.cpp src/Hamming/Hamming_avx512_16.cpp \
src/XOR/XOR_base.cpp \
src/XOR/XOR_scalar_16_16.cpp src/XOR/XOR_scalar_16_8.cpp src/XOR/XOR_scalar_32_32.cpp src/XOR/XOR_scalar_32_8.cpp \
src/XOR/XOR_simd.cpp src/XOR/XOR_sse42_4x32_32.cpp src/XOR/XOR_sse42_4x32_4x32.cpp src/XOR/XOR_sse42_8x16_16.cpp src/XOR/XOR_sse42_8x16_8x16.cpp \
src/XOR/XOR_avx2_16x16_16.cpp src/XOR/XOR_avx2_16x16_16x16.cpp src/XOR/XOR_avx2_8x32_32.cpp src/XOR/XOR_avx2_8x32_8x32.cpp \
src/AN/AN_scalar_8_16_u_inv.cpp src/AN/AN_scalar_8_16_s_inv.cpp \
src/AN/AN_scalar_16_32_s_divmod.cpp src/AN/AN_scalar_16_32_s_inv.cpp src/AN/AN_scalar_16_32_u_divmod.cpp src/AN/AN_scalar_16_32_u_inv.cpp \
src/AN/AN_scalar_32_64_s_inv.cpp src/AN/AN_scalar_32_64_u_inv.cpp \
src/AN/AN_sse42_16_32_s_divmod.cpp src/AN/AN_sse42_16_32_s_inv.cpp src/AN/AN_sse42_16_32_u_divmod.cpp src/AN/AN_sse42_16_32_u_inv.cpp \
src/AN/AN_avx2_16_32_s_divmod.cpp src/AN/AN_avx2_16_32_s_inv.cpp src/AN/AN_avx2_16_32_u_divmod.cpp src/AN/AN_avx2_16_32_u_inv.cpp \
src/CRC/CRC_base.cpp src/CRC/CRC32_scalar_16.cpp src/CRC/CRC32_scalar_32.cpp" "")

add_algorithm(AN_Datalength_Comparison "src/AN_Datalength_Comparison.cpp src/Util/Test.cpp" benchbase)

add_algorithm(benchmark "src/Main.cpp src/Util/Test.cpp" benchbase)

add_algorithm(benchmark32 "src/Main.cpp src/Util/Test.cpp" benchbase)
TARGET_COMPILE_DEFINITIONS(benchmark32 PRIVATE "TEST32")

add_algorithm(benchmark_novec "src/Main.cpp src/Util/Test.cpp" benchbase)
TARGET_COMPILE_OPTIONS(benchmark_novec PUBLIC "-fno-tree-vectorize")

add_algorithm(benchmark32_novec "src/Main.cpp src/Util/Test.cpp" benchbase)
TARGET_COMPILE_DEFINITIONS(benchmark32_novec PRIVATE "TEST32")
TARGET_COMPILE_OPTIONS(benchmark32_novec PUBLIC "-fno-tree-vectorize")

FIND_PACKAGE(OpenMP)
IF(OPENMP_FOUND)
	add_algorithm(benchmarkOMP2 "src/Main.cpp src/Util/Test.cpp" benchbase)
	TARGET_COMPILE_DEFINITIONS(benchmarkOMP2 PRIVATE "OMP" "OMPNUMTHREADS=2")
	append_target_property(benchmarkOMP2 "COMPILE_FLAGS" "${OpenMP_CXX_FLAGS}")
	append_target_property(benchmarkOMP2 "LINK_FLAGS" "${OpenMP_CXX_FLAGS}")

	add_algorithm(benchmarkOMP "src/Main.cpp src/Util/Test.cpp" benchbase)
	TARGET_COMPILE_DEFINITIONS(benchmarkOMP PRIVATE "OMP")
	append_target_property(benchmarkOMP "COMPILE_FLAGS" "${OpenMP_CXX_FLAGS}")
	append_target_property(benchmarkOMP "LINK_FLAGS" "${OpenMP_CXX_FLAGS}")
ENDIF(OPENMP_FOUND)

FIND_PACKAGE(Boost)
IF(Boost_FOUND)
    add_algorithm(TestModuloInverseComputation "src/TestModuloInverseComputation.cpp src/Util/Stopwatch.cpp")
    add_algorithm(TestModuloInverseComputation2 "src/TestModuloInverseComputation2.cpp src/Util/Stopwatch.cpp")
ELSE(Boost_FOUND)
    MESSAGE(WARNING "Boost not found, NOT generating targets 'TestModuloInverseComputation' and 'TestModuloInverseComputation2'")
ENDIF(Boost_FOUND)

add_algorithm(TestComputeHamming "src/TestComputeHamming.cpp src/Util/Test.cpp" benchbase)
add_algorithm(TestComputeHamming_novec "src/TestComputeHamming.cpp src/Util/Test.cpp" benchbase)
TARGET_COMPILE_OPTIONS(TestComputeHamming_novec PUBLIC "-fno-tree-vectorize")

add_algorithm(TestPopcount "src/TestPopcount.cpp src/Util/Test.cpp" benchbase)
add_algorithm(TestPopcount_novec "src/TestPopcount.cpp src/Util/Test.cpp" benchbase)
TARGET_COMPILE_OPTIONS(TestPopcount_novec PUBLIC "-fno-tree-vectorize")
