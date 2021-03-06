#!/bin/sh

#echo "$@"
#exit 1

UNITS=(kodi emulationstation none)

function process_action {
  if [ "$2" != "-1" ] && [ "${UNITS[$2]}" != "none" ]; then
    echo "${1}ing ${UNITS[$2]}"
    systemctl $1 ${UNITS[$2]}
  else
    echo "ignored $1 state $2"
  fi
}

process_action stop  $1
process_action start $2

