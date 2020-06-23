#!/bin/bash
filename1="/sys/devices/gpu.0/load"
while true
do
sleep 1
 while read line
  do
   echo "$line"
 done < $filename1
done
