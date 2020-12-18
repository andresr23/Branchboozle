#!/bin/bash
PLATFORM=${1:-"skylake"}

EXPERIMENT="sl"

BINARY="${EXPERIMENT}.out"
GNPLOT="${EXPERIMENT}.gp"
FINALF="${EXPERIMENT}-${PLATFORM}.dat"

taskset -c 0 ./$BINARY > $FINALF
wait

gnuplot -e "platform='$PLATFORM'" $GNPLOT
