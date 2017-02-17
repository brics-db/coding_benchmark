#!/usr/bin/env /bin/bash

# clean
rm -Rf build

# build
mkdir build
pushd build
(cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-6 .. && make -j $(nproc)) || exit 1

#run
./benchmark >benchmark.csv 2>benchmark.err

# eval / plot
HAS_AVX2=$(grep -m 1 flags /proc/cpuinfo | grep avx2)
if [[ -z "${HAS_AVX2}" ]]; then
	gnuplot ../plot_encode.m
	gnuplot ../plot_check.m
	gnuplot ../plot_decode.m
else
	gnuplot ../plot_encode_avx2.m
	gnuplot ../plot_check_avx2.m
	gnuplot ../plot_decode_avx2.m
fi

#run OMP (2 threads only)
./benchmarkOMP2 >benchmarkOMP2.csv 2>benchmarkOMP2.err

#run OMP (all threads available)
./benchmarkOMP >benchmarkOMP.csv 2>benchmarkOMP.err

popd
