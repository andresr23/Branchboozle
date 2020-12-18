#!/bin/bash
PLATFORM=${1:-"skylake"}

EXPERIMENT="pht3b-covertc-rdtsc"

BINARY="${EXPERIMENT}.out"
FINALF="${EXPERIMENT}-${PLATFORM}.dat"

taskset -c 0 ./$BINARY > $FINALF
wait
