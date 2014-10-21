#!/bin/sh
gnuplot << EOF

reset
unset key; set xtics nomirror; set ytics nomirror; set border front;

color(name) = (name eq "http") ? 0xff0000 \
	: (name eq "https") ? 0x00ff00 \
	: (name eq "h2c") ? 0x0000ff \
	: (name eq "h2") ? 0xffff00 \
	: (name eq "black") ? 0x000000 \
	: int(rand(0)*0xffffff)

set auto x
set grid
set title "Page load performances"
set xlabel "Protocol"
set ylabel "Time in seconds"

# Sets the y range from 0 to max value of y
set yrange [0:*]

set style fill solid

plot '$1' u (column(0)):2:(0.5):(color(strcol(1))):xtic(1) w boxes fillcolor rgb variable 
load "loop_til_escape"
EOF
