#!/bin/bash
EXPERIMENT="bhb-simulation"

BINARY="${EXPERIMENT}.out"
FINALF="${EXPERIMENT}.dat"

./$BINARY > $FINALF
wait
