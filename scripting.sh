#!/bin/sh
i=1
while [ $i -le 10 ]
do
  sh -c "echo 'ping -c 4 8.8.8.8' >> ./test"
  rename test test_$i test
  chmod +x test_$i
  sh test_$i
  usleep 0.001
  rm -rf test_$i
  echo $i
  let i++
done

