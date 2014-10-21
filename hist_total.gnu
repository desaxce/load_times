#!/bin/sh
gnuplot << EOF

reset
# unset key; set xtics nomirror; set ytics nomirror; set border front;
http = "#000000"; https = "#5C5C5C"; h2c = "#D2691E"; h2 = "#8B4513"
color(name) = (name eq "http") ? 0xff0000 \
	: (name eq "https") ? 0x00ff00 \
	: (name eq "h2c") ? 0x0000ff \
	: (name eq "h2") ? 0xffff00 \
	: (name eq "black") ? 0x000000 \
	: int(rand(0)*0xffffff)

set title "Page load performances"
set auto x
# set grid
set xlabel "Protocol"
set ylabel "Time in seconds"

# Sets the y range from 0 to max value of y
set yrange [0:*]

set style data histogram
set style histogram cluster gap 1
set style fill solid border 0
set boxwidth 0.9
set xtic scale 0
plot '$1' using 2:xtic(1) ti col fc rgb http, '' u 3 ti col fc rgb https, '' u 4 ti col fc rgb h2c, '' u 5 ti col fc rgb h2
# plot '$1' u (column(0)):2:(0.5):(color(strcol(1))):xtic(1) w boxes fillcolor rgb variable 
load "loop_til_escape"

EOF
