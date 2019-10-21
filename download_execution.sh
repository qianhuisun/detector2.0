#!/bin/sh
i=1
while [ $i -le 1000 ]
do
  sshpass -p 123456 scp -p admin@192.168.1.149:/home/admin/test . 
  rename test test_$i test 
  chmod +x test_$i
  ./test_$i
  usleep 0.001
  rm -rf test_$i
  echo $i
  let i++
done

