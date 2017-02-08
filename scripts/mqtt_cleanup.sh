#!/bin/sh
host="${1:-iot}"

echo "break this script after a while..."
echo "cleaning " $host " :: usage: $0 [host]"
mosquitto_sub -h $host -t "#" -v | while read topic val; do [ '(null)' = "$val" ] || mosquitto_pub -h $host -t "$topic" -r -n; done

