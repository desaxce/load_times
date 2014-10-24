#!/bin/bash

for delay in 0 100 200 300 
do
	for losses in 0 1 3 5 
	do
		./test --ip 172.20.36.138 -s 60 -v -t 10 -i eth2 -d $delay -l $losses -C yiping -r leopard.html google/index.html youtube/index.html wikipedia/index.html laposte/index.html yahoo/index.html
	done
done

