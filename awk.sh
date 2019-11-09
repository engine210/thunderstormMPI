#!/bin/bash
awk -F"," 'NR==1{printf "\"time\",%s,%s,\"z\",%s,%s,%s,%s,%s\n",$2,$3,$4,$5,$6,$7,$8} NR>1{printf "1,%s,%s,%s,%s,%s,%s,%s,%s\n",$1,$2,$3,$4,$5,$6,$7,$8}' layer_100.csv > output.csv
