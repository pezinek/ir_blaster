#!/bin/sh

host="${1:?Specify MQTT host as first parameter}"
nodeid="${id?Specify node ID as configured}"

MSG='{"name":"UNKNOWN: 0x42D74094","value":1121403028,"frequency":38,"type":{"id":-1,"name":"UNKNOWN"},"raw":[9100,4600,600,650,600,1750,600,1750,600,1750,600,650,600,1750,600,1750,600,1750,600,1750,600,1750,600,1750,600,650,600,650,600,650,600,650,600,1800,600,650,600,650,600,1750,600,650,600,650,600,650,600,650,600,650,600,1750,600,1750,600,650,600,650,600,1750,600,1750,600,650,600,650,600]}';

mosquitto_pub -h "${host}" -t "homie/${nodeid}/infrared/code/set" -m "${MSG}";

