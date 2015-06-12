#!/bin/sh

echo "$@"

UNITS=(kodi emulatorstation)

[[ "$1" -ne "-1" ]] && systemctl stop ${UNITS[$1]}
[[ "$2" -ne "-1" ]] && systemctl start ${UNITS[$2]}

