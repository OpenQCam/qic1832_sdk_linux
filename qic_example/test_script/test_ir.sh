#!/bin/bash

while :
do
./example_control -s "30 0"
sleep 1
./example_control -s "30 1"
sleep 1
done
