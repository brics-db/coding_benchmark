#!/usr/bin/env gnuplot

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

set term pdf enhanced
set datafile separator ','
set key outside
set log x 2
set xrange [1:1024]
set xtics 1,2,1024
set format x "2^{%L}"
set xlabel "unroll / block size"
set ylabel "Runtime [ns]"

set output 'plot_check_16bit.pdf'
plot for [col=19:31] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_16bit_noHamming.pdf'
plot for [col=19:30] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_32bit.pdf'
plot 'benchmark.csv' using 1:2 lw 2 ps 0.5 with linespoints title columnhead,
	for [col=32:35] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_32bit_noHamming.pdf'
plot for [col=32:34] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_novec_16bit.pdf'
plot for [col=19:31] 'benchmark_novec.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_novec_16bit_noHamming.pdf'
plot for [col=19:30] 'benchmark_novec.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_novec_32bit.pdf'
plot for [col=32:35] 'benchmark_novec.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_novec_32bit_noHamming.pdf'
plot for [col=32:34] 'benchmark_novec.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead
