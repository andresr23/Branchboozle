#!/bin/bash
PLATFORM=${1:-"skylake"}

EXPERIMENT="pht2b-primeprobe"

BINARY="${EXPERIMENT}.out"
FINALF="${EXPERIMENT}-${PLATFORM}.dat"

taskset -c 0 ./$BINARY > $FINALF
wait
