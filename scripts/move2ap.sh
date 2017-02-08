#!/bin/sh

host="${1?MQTT host required as first argument}"
node="${2?Node ID required as second argument}"
ssid="${3?SSID name of target wifi required}"
passwd="${4?Password for target wifi required}"

mosquitto_pub -h $host -t "homie/$node/\$implementation/config/set" -m "{ 'wifi': { 'ssid': '${ssid}', 'password': '${passwd}' }}"

