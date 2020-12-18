set terminal png font 'Helvetica, 19' size 1920,590 enhanced
if(!exists("platform")) platform = "skylake"
set output 'pht3b-rdtsc-measurement-'.platform.'.png'

set key box
set grid

_x = 10000
_y = 500

set xrange[1:_x]
set xtics 0,1000,_x

set yrange[-0.5:_y + 0.5]
set ytics 0,50,_y

set multiplot layout 1,2

## ST
set title 'Probe Non-Taken'
plot 'pht3b-rdtsc-measurement-'.platform.'.dat' using 1 with points title 'Miss' lc 'red'  , \
     'pht3b-rdtsc-measurement-'.platform.'.dat' using 2 with points title 'Hit'  lc 'blue'

## SN
set title 'Probe Taken'
plot 'pht3b-rdtsc-measurement-'.platform.'.dat' using 3 with points title 'Miss' lc 'red'  , \
     'pht3b-rdtsc-measurement-'.platform.'.dat' using 4 with points title 'Hit'  lc 'blue'

unset multiplot
