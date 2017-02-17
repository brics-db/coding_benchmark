#!/usr/bin/env gnuplot

# Copyright 2016 Till Kolditz
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

set term pdf color enhanced
set datafile separator ','

set log x 2
set xrange [1:1024]
set xtics 1,4,1024
set format x "2^{%L}"
set xlabel "unroll / block size"

set key outside

#Absolute values
set ylabel "Runtime [ns]"

set output 'plot_decode_16bit_absolute_seq.pdf'
plot for [col=29:31] 'benchmark.csv' index 0 using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	'benchmark.csv' index 0 using 1:34 lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_absolute_seq_inv.pdf'
plot for [col=29:34] 'benchmark.csv' index 0 using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_absolute_vec.pdf'
plot for [col=35:37] 'benchmark.csv' index 0 using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	'benchmark.csv' index 0 using 1:40 lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_absolute_vec_inv.pdf'
plot for [col=35:40] 'benchmark.csv' index 0 using 1:col lw 2 ps 0.5 with linespoints title columnhead

#Relative values
set ylabel "Relative Slowdown"

set output 'plot_decode_16bit_relative_seq.pdf'
plot for [col=29:31] 'benchmark.csv' index 1 using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	'benchmark.csv' index 1 using 1:34 lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_relative_seq_inv.pdf'
plot for [col=29:34] 'benchmark.csv' index 1 using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_relative_vec.pdf'
plot for [col=35:37] 'benchmark.csv' index 1 using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	'benchmark.csv' index 1 using 1:40 lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_decode_16bit_relative_vec_inv.pdf'
plot for [col=35:40] 'benchmark.csv' index 1 using 1:col lw 2 ps 0.5 with linespoints title columnhead

###############
# Paper-plots #
###############
reset
set terminal cairolatex pdf input blacktext color fontscale 0.44 transparent size 1.625in,1.25in
set datafile separator ','
set log x 2
set xrange [1:1024]
set format x "$2^{%L}$"
set log x2 2
set x2range [1:1024]
set xtics 1,4,1024
set xlabel "unroll / block size"
set ytics autofreq
set yrange [*:*]
unset ylabel
set border 1+2+4+8
unset key
unset label
unset arrow

set style line 1 lt 1 lw 2 ps 0.5
set style line 2 lt 2 lw 2 ps 0.5
set style line 3 lt 3 lw 2 ps 0.5
set style line 4 lt 4 lw 2 ps 0.5
set style line 5 lt 5 lw 2 ps 0.5
set style line 6 lt 6 lw 2 ps 0.5
set style line 7 lt 7 lw 2 ps 0.5
set style line 8 lt 8 lw 2 ps 0.5
set style line 9 lt 9 lw 2 ps 0.5

# 1 XOR      seq / SSE4.2
# 3 Hamming  seq / SSE4.2
# 4 AN inv U seq / SSE4.2
# 5 AN inv S seq / SSE4.2

set output 'plot_paper_decode_16bit_seq.tex'
plot 'benchmark.csv' index 0 using 1:29 ls 1 with linespoints,\
	'benchmark.csv' index 0 using 1:30 ls 4 with linespoints,\
	'benchmark.csv' index 0 using 1:31 ls 5 with linespoints,\
	'benchmark.csv' index 0 using 1:34 ls 3 with linespoints

set output 'plot_paper_decode_16bit_seq_inv.tex'
plot 'benchmark.csv' index 0 using 1:29 ls 1 with linespoints,\
	'benchmark.csv' index 0 using 1:32 ls 4 with linespoints,\
	'benchmark.csv' index 0 using 1:33 ls 5 with linespoints

set output 'plot_paper_decode_16bit_vec.tex'
plot 'benchmark.csv' index 0 using 1:35 ls 1 with linespoints,\
	'benchmark.csv' index 0 using 1:36 ls 4 with linespoints,\
	'benchmark.csv' index 0 using 1:37 ls 5 with linespoints,\
	'benchmark.csv' index 0 using 1:40 ls 3 with linespoints

set output 'plot_paper_decode_16bit_vec_inv.tex'
plot 'benchmark.csv' index 0 using 1:35 ls 1 with linespoints,\
	'benchmark.csv' index 0 using 1:38 ls 4 with linespoints,\
	'benchmark.csv' index 0 using 1:39 ls 5 with linespoints

reset
set bmargin 0
set lmargin 0
set rmargin 0
set tmargin 0
set noborder
set noxtics
set noytics
set noxlabel
set noylabel
set notitle
unset label
unset arrow
set xrange [-20:-10]
set yrange [-20:-10]
		
set term cairolatex pdf input blacktext color fontscale 0.44 transparent size 3.2in,.15in
set output 'plot_paper_legend_decode.tex'
set key below maxcols 3 maxrows 1 horizontal width 4
plot 20 ls 1 t "XOR" with linespoints,\
	20 ls 4 t "AN$_U$" with linespoints,\
	20 ls 5 t "AN$_S$" with linespoints,\
	20 ls 3 t "Hamming" with linespoints

set output 'plot_paper_legend_decode_inv.tex'
set key below maxcols 2 maxrows 1 horizontal width 0.5
plot 20 ls 1 t "XOR" with linespoints,\
	20 ls 4 t "AN$_U$" with linespoints,\
	20 ls 5 t "AN$_S$" with linespoints
