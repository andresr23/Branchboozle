set terminal png font 'Helvetica, 19' size 960,1350 enhanced
if(!exists("platform")) platform = "skylake"
set output 'hs-implementation-'.platform.'.png'

unset key

_x = 120
_y = 1

set xrange[0.5:_x + 0.5]
set xtics 0,5,_x font ',13'
set xlabel 'History Saturation (s)' font ',13'

set yrange[-0.5:_y + 0.5]
set ytics 0,1,_y font ',13'
set ylabel 'Probe ' font ',13'

set multiplot layout 4,1

## Instruction
set title 'loop Instruction'
plot 'hs-implementation-'.platform.'.dat' using (1+$0):($1) with linespoints

## While
set title 'While'
plot 'hs-implementation-'.platform.'.dat' using (1+$0):($2) with linespoints

## For
set title 'For'
plot 'hs-implementation-'.platform.'.dat' using (1+$0):($3) with linespoints

## Function
set title 'Function'
plot 'hs-implementation-'.platform.'.dat' using (1+$0):($4) with linespoints

unset multiplot
