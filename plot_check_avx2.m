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
# distributed under the License is distributed on an "AS IS" BASIS,\
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
set ylabel "Relative Slowdown"

set key outside

set output 'plot_check_16bit_seq.pdf'
plot for [col=29:34] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	'benchmark.csv' using 1:39 lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_16bit_seq_inv.pdf'
plot for [col=29:39] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_16bit_vec.pdf'
plot 'benchmark.csv' using 1:29 lw 2 ps 0.5 with linespoints title columnhead,\
	for [col=40:42] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead,\
	for [col=45:48] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_16bit_vec_inv.pdf'
plot 'benchmark.csv' using 1:29 lw 2 ps 0.5 with linespoints title columnhead,\
	for [col=40:50] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_32bit_seq.pdf'
plot 'benchmark.csv' using 1:29 lw 2 ps 0.5 with linespoints title columnhead,\
	for [col=51:52] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

set output 'plot_check_32bit_vec.pdf'
plot 'benchmark.csv' using 1:29 lw 2 ps 0.5 with linespoints title columnhead,\
	for [col=53:55] 'benchmark.csv' using 1:col lw 2 ps 0.5 with linespoints title columnhead

###############
# Paper-plots #
###############
reset
#set term pdf color enhanced size 3in,2in
set term cairolatex pdf input blacktext color fontscale 0.44 transparent size 1.625in,1.25in
set datafile separator ','
set log x 2
set xrange [1:1024]
set format x "$2^{%L}$"
set log x2 2
set x2range [1:1024]

set style line 1 lt 1 lw 2 ps 0.5
set style line 2 lt 2 lw 2 ps 0.5
set style line 3 lt 3 lw 2 ps 0.5
set style line 4 lt 4 lw 2 ps 0.5
set style line 5 lt 5 lw 2 ps 0.5
set style line 6 lt 6 lw 2 ps 0.5
set style line 7 lt 7 lw 2 ps 0.5
set style line 8 lt 8 lw 2 ps 0.5
set style line 9 lt 9 lw 2 ps 0.5
set style line 10 lt 10 lw 2 ps 0.5

bm = 0.20
lm = 0.20
rm = 0.95
gap = 0.05
size = 0.70
y1 = 0.0; y2 = 11.5; y3 = 28.5; y4 = 50.0

set output 'plot_paper_check_16bit_seq.tex'
set multiplot
set border 1+2+8
unset key
unset label
unset arrow
set xtics nomirror 1,4,1024
unset x2tics
set xlabel "unroll / block size"
set ytics 5
unset ylabel
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) )
set yrange [y1:y2]
plot 'benchmark.csv' using 1:29 ls 1 with linespoints,\
	'benchmark.csv' using 1:30 ls 2 with linespoints
unset xtics
unset xlabel
unset ylabel
set x2tics 1,4,1024
set format x2 ""
set border 2+4+8
set bmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap
set tmargin at screen bm + size + gap
set yrange [y3:y4]
#set label 'Relative Slowdown' at screen 0.03, bm + 0.5 * (size + gap) offset 0,-strlen("Relative Slowdown")/4.0 rotate by 90
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
plot 'benchmark.csv' using 1:31 ls 3 with linespoints,\
	'benchmark.csv' using 1:39 ls 4 with linespoints
unset multiplot



set output 'plot_paper_check_16bit_seq_inv.tex'
set multiplot
set border 1+2+8
unset key
unset label
unset arrow
set xtics nomirror 1,4,1024
unset x2tics
set xlabel "unroll / block size"
set ytics 5
unset ylabel
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) )
set yrange [y1:y2]
plot 'benchmark.csv' using 1:29 ls 1 with linespoints,\
	'benchmark.csv' using 1:30 ls 2 with linespoints,\
	'benchmark.csv' using 1:35 ls 6 with linespoints,\
	'benchmark.csv' using 1:36 ls 7 with linespoints
unset xtics
unset xlabel
unset ylabel
set x2tics 1,4,1024
set format x2 ""
set border 2+4+8
set bmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap
set tmargin at screen bm + size + gap
set yrange [y3:y4]
#set label 'Relative Slowdown' at screen 0.03, bm + 0.5 * (size + gap) offset 0,-strlen("Relative Slowdown")/4.0 rotate by 90
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
plot 'benchmark.csv' using 1:31 ls 6 with linespoints,\
plot 'benchmark.csv' using 1:32 ls 7 with linespoints,\
unset multiplot

set output 'plot_paper_check_16bit_vec.tex'
set multiplot
set border 1+2+8
unset key
unset label
unset arrow
set xtics nomirror 1,4,1024
unset x2tics
set xlabel "unroll / block size"
set ytics 5
unset ylabel
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) )
set yrange [y1:y2]
plot 'benchmark.csv' using 1:29 ls 1 with linespoints,\
	'benchmark.csv' using 1:40 ls 5 with linespoints,\
	'benchmark.csv' using 1:46 ls 8 with linespoints
unset xtics
unset xlabel
unset ylabel
set x2tics 1,4,1024
set format x2 ""
set border 2+4+8
set bmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap
set tmargin at screen bm + size + gap
set yrange [y3:y4]
#set label 'Relative Slowdown' at screen 0.03, bm + 0.5 * (size + gap) offset 0,-strlen("Relative Slowdown")/4.0 rotate by 90
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
plot 'benchmark.csv' using 1:41 ls 9 with linespoints,\
	'benchmark.csv' using 1:45 ls 4 with linespoints,\
	'benchmark.csv' using 1:47 ls 10 with linespoints
unset multiplot

y1 = 0.0; y2 = 11.5; y3 = 28.5; y4 = 50.0
set output 'plot_paper_check_16bit_vec_inv.tex'
set multiplot
set border 1+2+8
unset key
unset label
unset arrow
set xtics nomirror 1,4,1024
unset x2tics
set xlabel "unroll / block size"
set ytics 5
unset ylabel
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) )
set yrange [y1:y2]
plot 'benchmark.csv' using 1:29 ls 1 with linespoints,\
	'benchmark.csv' using 1:40 ls 5 with linespoints,\
	'benchmark.csv' using 1:43 ls 9 with linespoints,\
	'benchmark.csv' using 1:46 ls 8 with linespoints,\
	'benchmark.csv' using 1:49 ls 10 with linespoints
unset xtics
unset xlabel
unset ylabel
set x2tics 1,4,1024
set format x2 ""
set border 2+4+8
set bmargin at screen bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap
set tmargin at screen bm + size + gap
set yrange [y3:y4]
#set label 'Relative Slowdown' at screen 0.03, bm + 0.5 * (size + gap) offset 0,-strlen("Relative Slowdown")/4.0 rotate by 90
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen lm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	lm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0 to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 nohead
set arrow from screen rm - gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1)+abs(y4-y3) ) ) - gap / 4.0  + gap to screen \
	rm + gap / 4.0, bm + size * (abs(y2-y1) / (abs(y2-y1) + abs(y4-y3) ) ) + gap / 4.0 + gap nohead
plot 'benchmark.csv' using 1:41 ls 9 with linespoints
unset multiplot

set output 'plot_paper_check_16bit_seq_inv_zoom.tex'
set border 1+2+4+8
unset key
unset label
unset arrow
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size
set yrange [0.25:10.25]
set ytics 2
set xtics nomirror 1,4,1024
plot 'benchmark.csv' using 1:23 ls 1 with linespoints,\
	'benchmark.csv' using 1:24 ls 2 with linespoints,\
	'benchmark.csv' using 1:29 ls 5 with linespoints,\
	'benchmark.csv' using 1:30 ls 6 with linespoints

set output 'plot_paper_check_16bit_vec_inv_zoom.tex'
set border 1+2+4+8
unset key
unset label
unset arrow
set lmargin at screen lm
set rmargin at screen rm
set bmargin at screen bm
set tmargin at screen bm + size
set yrange [0.25:2.75]
set ytics 0.5
set xtics nomirror 1,4,1024
plot 'benchmark.csv' using 1:23 ls 1 with linespoints,\
	'benchmark.csv' using 1:34 ls 2 with linespoints,\
	'benchmark.csv' using 1:37 ls 5 with linespoints

set term cairolatex pdf input blacktext color fontscale 0.44 transparent size .2in,1.25in
set output 'plot_paper_xlabel.tex'
unset border
unset xtics
unset ytics
unset x2tics
unset y2tics
unset xlabel
unset x2label
unset ylabel
unset y2label
unset label
unset arrow
unset key
set label 'Relative Slowdown' at screen 0.5, bm + 0.4 * (size + gap) offset 0,-strlen("Relative Slowdown")/4.0 rotate by 90
plot 1 ls 0 with linespoints

set term cairolatex pdf input blacktext color fontscale 0.44 transparent size 6.7in,.2in
set output 'plot_paper_legend_check.tex'
unset border
unset xtics
unset xlabel
unset x2tics
unset x2label
unset ytics
unset ylabel
unset y2tics
unset y2label
unset label
unset arrow
set key below center width 5
plot  NaN ls 1 t "memcmp" with linespoints,\
	NaN ls 2 t "XOR seq" with linespoints,\
	NaN ls 5 t "XOR SSE" with linespoints,\
	NaN ls 8 t "XOR AVX2" with linespoints,\
	NaN ls 3 t "AN" with linespoints,\
	NaN ls 6 t "AN$_{U,\text{seq}}$" with linespoints,\
	NaN ls 7 t "AN$_{S,\text{seq}}$" with linespoints,\
	NaN ls 9 t "AN$_{U,\text{SSE}}$" with linespoints,\
	NaN ls 10 t "AN$_{U,\text{AVX2}}$" with linespoints,\
	NaN ls 4 t "Hamming" with linespoints

set output 'plot_paper_legend_check_inv.tex'
unset border
unset xtics
unset xlabel
unset x2tics
unset x2label
unset ytics
unset ylabel
unset y2tics
unset y2label
unset label
unset arrow
set key below center width 5
plot  NaN ls 1 t "memcmp" with linespoints,\
	NaN ls 2 t "XOR seq" with linespoints,\
	NaN ls 5 t "XOR SSE" with linespoints,\
	NaN ls 8 t "XOR AVX2" with linespoints,\
	NaN ls 3 t "AN" with linespoints,\
	NaN ls 6 t "AN$_{U,\text{seq}}$" with linespoints,\
	NaN ls 7 t "AN$_{S,\text{seq}}$" with linespoints,\
	NaN ls 9 t "AN$_{U,\text{SSE}}$" with linespoints,\
	NaN ls 10 t "AN$_{U,\text{AVX2}}$" with linespoints,\
	NaN ls 4 t "Hamming" with linespoints
