#!/bin/bash
PLATFORM=${1:-"skylake"}

EXPERIMENT="pht2b-states"

BINARY="${EXPERIMENT}.out"
RESULT="${EXPERIMENT}-${PLATFORM}.dat"

taskset -c 0 ./$BINARY > $RESULT
wait
