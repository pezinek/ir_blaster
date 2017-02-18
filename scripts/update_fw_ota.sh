#!/bin/sh

fw="${1:?Complied binary with the firmware required as first argument (press Ctrl-Alt-S from arduino GUI)}"
deviceid="${2:?Device ID required as second argument}"
host="${3:?MQTT server required as third argument}"

sum=$(md5sum "$fw" | awk 'NR==1 { print $1 }')
echo $sum

mosquitto_pub -h "$host" -t "homie/${deviceid}/\$implementation/ota/checksum" -m "$sum"
sleep 1
mosquitto_pub -h "$host" -t "homie/${deviceid}/\$implementation/ota/firmware" -f "$fw"

