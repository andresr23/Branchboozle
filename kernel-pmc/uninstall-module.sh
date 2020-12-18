#!/bin/bash
[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

rmmod pmc-module

if [ "$1" == "dev" ]; then
  echo 1 > /sys/devices/cpu/rdpmc
fi
