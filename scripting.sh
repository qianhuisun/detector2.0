#!/bin/sh
i=1
while [ $i -le 10 ]
do
  sh -c "echo 'ping -c 4 8.8.8.8' >> ./test.sh"
  rename test.sh test_$i.sh test.sh
  chmod +x test_$i.sh
  sh test_$i.sh
  usleep 0.001
  rm -rf test_$i.sh
  echo $i
  let i++
done

