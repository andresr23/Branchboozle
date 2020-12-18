set terminal png font 'Helvetica, 19' size 960,300 enhanced
if(!exists("platform")) platform = "skylake"
set output 'hs-miss-'.platform.'.png'

unset key

_x = 120
_y = 2

set xrange[0.5:_x + 0.5]
set xtics 0,5,_x font ',13'
set xlabel 'History Saturation (s)' font ',13'

set yrange[-0.5:_y + 0.5]
set ytics 0,1,_y font ',13'
set ylabel 'Misprediction Count (PMC)' font ',13'

plot 'hs-miss-'.platform.'.dat' using (1+$0):($1) title 'Branch Mis-predicted' with linespoints
