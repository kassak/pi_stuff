#!/bin/sh

REG='sudo insmod remeasure.ko debug=1 gpio_a_pin=20 gpio_b_pin=21 measure_timeout_usec=-1 min_relaxation_usec=5000000'
UNREG='sudo rmmod remeasure'
if [ "$1" == "1" ]; then
  $($REG)
elif [ "$1" == "2" ]; then
  $($UNREG)
else
  $($REG) && $($UNREG)
fi
