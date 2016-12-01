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

set output 'plot_check.pdf'
#plot for [col=12:21] 'an_vs_xor.csv' using 1:col with lines title columnhead
plot for [col=12:21] 'an_vs_xor.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_novec.pdf'
plot for [col=12:21] 'an_vs_xor_novec.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead
