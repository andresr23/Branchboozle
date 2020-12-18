set terminal png font 'Helvetica, 19' size 960,600 enhanced
if(!exists("platform")) platform = "skylake"
set output 'sl-'.platform.'.png'

unset key
set grid

_x = 20
_y = 10

set xrange[1:_x]
set xtics 0,1,20
set xlabel 'Iterations' font ',13'

set yrange[-0.5:_y + 0.5]
set ytics 0,1,10
set ylabel 'Mis-prediction Count (PMC)' font ',13'

plot 'sl-'.platform.'.dat' using (1+$0):($1) with lines
