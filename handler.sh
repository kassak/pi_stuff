#!/bin/sh

echo "$@"

UNITS=(kodi emulatorstation)

[[ "$1" -ne "-1" ]] && echo ${UNITS[$1]} && systemctl stop ${UNITS[$1]}
[[ "$2" -ne "-1" ]] && echo ${UNITS[$2]} && systemctl start ${UNITS[$2]}

