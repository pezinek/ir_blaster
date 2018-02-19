#!/bin/sh

host="${1:?Specify MQTT host as first parameter}"
nodeid="${id?Specify node ID as configured}"

MSG='{"name":"UNKNOWN: 0x78D2331A","value":2027041562,"frequency":38,"type":{"id":-1,"name":"UNKNOWN"},"raw":[200,1100,200,2350,200,950,200,2900,200,1350,200,1350,200,950,200,2200,200,13850,200,1100,200,2750,200,800,200,950,200,800,200,2900,200,800,200,800,200]}'

mosquitto_pub -h "${host}" -t "homie/${nodeid}/infrared/code/set" -m "${MSG}";

