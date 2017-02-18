#!/usr/bin/env python


import sys
import json

if len(sys.argv) < 4:
	print ("The pronto code needs to be passed on commandlie")
	sys.exit(69)


code_type = sys.argv[1]
code_freq = 1000000 / (int(sys.argv[2], 16) * 0.241246)
num_pairs_1 = int(sys.argv[3], 16)
num_pairs_2 = int(sys.argv[4], 16)
period_time = 1000000 / code_freq

pairs_1 = []
pairs_2 = []

for offset in range(5, 5+num_pairs_1):
	pairs_1.append(int(sys.argv[offset], 16))	

for offset in range(5+num_pairs_1, 5+num_pairs_1+num_pairs_2):
	pairs_2.append(int(sys.argv[offset], 16))

out = {}
out["frequency"] = int(code_freq/1000)
out["raw"] = []
for t in pairs_1:
	out["raw"].append(int(1000000*t/code_freq))

print(json.dumps(out))

out["raw"] = []
for t in pairs_2:
	out["raw"].append(int(1000000*t/code_freq))

print(json.dumps(out))
