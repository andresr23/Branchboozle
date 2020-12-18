#!/bin/bash
[ "$UID" -eq 0 ] || exec sudo bash "$0" "$@"

if [ "$1" == "dev" ]; then
  echo 2 > /sys/devices/cpu/rdpmc
fi

cd module && insmod pmc-module.ko
