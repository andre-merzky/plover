#!/bin/sh

export LD_LIBRARY_PATH=`pwd`/engine

for i in 0 1 2 3 4 5 6 7 8 9 ; 
  do 
  for j in 0 1 2 3 4 5 6 7 8 9 ; 
   do 
    for k in 0 1 2 3 4 5 6 7 8 9 ; 
      do 
      echo $i$j$k;
      ./adaptor_test >/dev/null; 
    done
  done
done

