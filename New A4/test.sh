#!/bin/bash

PREFIX=test
TESTS='00 01 02 03 04 05 06 07 08 09'
PROG=runme.sh
count=0
DEBUG=0

if [ "$1" == "debug" ]; then
  let "DEBUG=1"
fi

if [ ! -x $PROG ]; then
  echo Error: $PROG is not in the current directory or is not executable
  exit 1
fi

for T in $TESTS; do
  if [ -f $PREFIX.$T.in ]; then  
    echo ===========================================
    echo Test file: $PREFIX.$T.in 
    ./$PROG < $PREFIX.$T.in > $PREFIX.$T.out
    if diff $PREFIX.$T.out $PREFIX.$T.gold > /dev/null; then
      echo " " PASSED
      let "count = count + 1"
      rm $PREFIX.$T.out
    elif [ $DEBUG -eq 1 ]; then
        echo Test $T failed, here are the differences
        echo My program output "                     " Expected program output
        echo ================= "                     " =======================
        diff -W 80 -y $PREFIX.$T.out $PREFIX.$T.gold | more
        exit 1
    else
      echo " " FAILED
    fi
  else
    echo Oops: file $PREFIX.$T.in does not exist!
  fi
done

echo =============================================
echo $count tests passed
