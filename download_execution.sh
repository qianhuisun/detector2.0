#!/bin/sh
i=1
while [ $i -le 10 ]
do
  wget -O test_$i https://raw.githubusercontent.com/redcanaryco/atomic-red-team/master/atomics/T1154/echo-art-fish.sh
  chmod +x test_$i
  ./test_$i
  # usleep 0.001
  rm -rf test_$i
  echo $i
  let i++
done
