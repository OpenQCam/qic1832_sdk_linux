#!/bin/sh
#get current resolution
./example_control -g 20

./example_control -s "24 0"

while :
do
./example_control -d 1 -s "20 15728960"
sleep 2
./example_control -d 1 -s "20 70780800"
sleep 2
done
