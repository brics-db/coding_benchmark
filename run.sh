#!/usr/bin/env /bin/bash

# clean
rm -Rf build
# build
mkdir build
pushd build
cmake ..
make
# run and set affinity to a single core
taskset -c 1 ./benchmark >benchmark.csv
taskset -c 1 ./benchmark_novec >benchmark_novec.csv
# eval / plot
HAS_AVX2=$(grep -m 1 flags /proc/cpuinfo | grep avx2)
if [[ -z "${HAS_AVX2}" ]]; then
	gnuplot ../plot_encode.m
	gnuplot ../plot_check.m
else
	gnuplot ../plot_encode_avx2.m
	gnuplot ../plot_check_avx2.m
fi
popd

