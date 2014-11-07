#!/bin/sh
gnuplot << EOF

set style fill solid 0.8 border -1
set boxwidth 1 relative

plot "output.txt" using 0:1 title 'Cleartext' with boxes
load "loop_til_escape"

EOF
