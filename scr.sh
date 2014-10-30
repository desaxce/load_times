#!/bin/bash

for delay in 0 100
do
	for losses in 0 5 
	do
		./test --ip 172.20.36.138 -s 60 -v -t 10 -i eth2 -d $delay -l $losses -C yiping -r leopard_better.html google/index.html youtube/index.html wikipedia/index.html yahoo/index.html
	done
done

