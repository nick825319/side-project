#!/bin/bash
filename1="/sys/bus/i2c/devices/6-0040/iio_device/rail_name_1"
filename2="/sys/bus/i2c/devices/6-0040/iio_device/in_power1_input"
filename3="/sys/bus/i2c/devices/6-0040/iio_device/rail_name_2"
filename4="/sys/bus/i2c/devices/6-0040/iio_device/in_power2_input"
filename5="/sys/bus/i2c/devices/6-0040/iio_device/rail_name_0"
filename6="/sys/bus/i2c/devices/6-0040/iio_device/in_power0_input"
while read line
do 
 s0=$line
done < $filename1
while read line
do 
 s1=$line
done < $filename3
while read line
do 
 s2=$line
done < $filename5
while true
do
sleep 1
 while read line
 do
  echo "$s0 $line"
 done < $filename2
 while read line
 do
  echo "$s1 $line"
 done < $filename4
 while read line
 do
  echo "$s2 $line"
 done < $filename6
done
