#!/usr/bin/env /bin/bash

# set CODING_BENCHMARK_BUILD for building
# set CODING_BENCHMARK_RUN for actual benchmarking

#########
# build #
#########
rm -Rf build
mkdir build
pushd build
(cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_COMPILER=g++-6 .. && make -j $(nproc)) || exit 1



##########
# normal #
##########
./benchmark >benchmark.out 2>benchmark.err
awk 'BEGIN{FS=",";} NF {if ($1 ~ /^(#|[^0-9])/){print;next}; if ($2 ~ /^[0-9]+\.[0-9]+$/) {print; next} {printf "%d",$1; for (i=2; i<=NF; i++){printf ",%.2f",($i/1000000000)}{printf "\n"}}; next} !NF {print}' benchmark.out >benchmark.out2
ln -s benchmark.out2 benchmark.csv
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
mkdir normal
mv plot* normal/
rm benchmark.csv



########################
# OMP (2 threads only) #
########################
./benchmarkOMP2 >benchmarkOMP2.out 2>benchmarkOMP2.err
awk 'BEGIN{FS=",";} NF {if ($1 ~ /^(#|[^0-9])/){print;next}; if ($2 ~ /^[0-9]+\.[0-9]+$/) {print; next} {printf "%d",$1; for (i=2; i<=NF; i++){printf ",%.2f",($i/1000000000)}{printf "\n"}}; next} !NF {print}' benchmarkOMP2.out >benchmarkOMP2.out2
ln -s benchmarkOMP2.out2 benchmark.csv
if [[ -z "${HAS_AVX2}" ]]; then
        gnuplot ../plot_encode.m
        gnuplot ../plot_check.m
        gnuplot ../plot_decode.m
else
        gnuplot ../plot_encode_avx2.m
        gnuplot ../plot_check_avx2.m
        gnuplot ../plot_decode_avx2.m
fi
mkdir OMP2
mv plot* OMP2/
rm benchmark.csv


###############################
# OMP (all threads available) #
###############################
if [[ 2 -ne "$(nproc)" ]]; then
	./benchmarkOMP >benchmarkOMP.out 2>benchmarkOMP.err
	awk 'BEGIN{FS=",";} NF {if ($1 ~ /^(#|[^0-9])/){print;next}; if ($2 ~ /^[0-9]+\.[0-9]+$/) {print; next} {printf "%d",$1; for (i=2; i<=NF; i++){printf ",%.2f",($i/1000000000)}{printf "\n"}}; next} !NF {print}' benchmarkOMP.out benchmarkOMP.out2
	ln -s benchmarkOMP.out2 benchmark.csv
	if [[ -z "${HAS_AVX2}" ]]; then
        	gnuplot ../plot_encode.m
	        gnuplot ../plot_check.m
	        gnuplot ../plot_decode.m
	else
	        gnuplot ../plot_encode_avx2.m
	        gnuplot ../plot_check_avx2.m
	        gnuplot ../plot_decode_avx2.m
	fi
	mkdir "OMP$(nproc)"
	mv plot* "OMP$(nproc)/"
	rm benchmark.csv
fi

popd

