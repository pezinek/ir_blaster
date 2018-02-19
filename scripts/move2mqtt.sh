#!/bin/sh

host="${1?MQTT host required as first argument}"
node="${2?Node ID required as second argument}"
new_host="${3:?New mqtt host required}"

mosquitto_pub -h $host -t "homie/$node/\$implementation/config/set" -m "{ 'mqtt': { 'host': '${new_host}' }}"

