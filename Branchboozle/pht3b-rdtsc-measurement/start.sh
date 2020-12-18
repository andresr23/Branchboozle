#!/bin/bash
PLATFORM=${1:-"skylake"}

EXPERIMENT="pht3b-rdtsc-measurement"

BINARY="${EXPERIMENT}.out"
GNPLOT="${EXPERIMENT}.gp"

OUTPUT0="${EXPERIMENT}.dat"
FINALF0="${EXPERIMENT}-${PLATFORM}.dat"
FINALF1="${EXPERIMENT}-report-${PLATFORM}.dat"

taskset -c 0 ./$BINARY > $FINALF1
wait

mv -f $OUTPUT0 $FINALF0

gnuplot -e "platform='$PLATFORM'" $GNPLOT
