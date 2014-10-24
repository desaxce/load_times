#!/bin/bash

for delay in 0 50 100 150 200 250 300
do
	for losses in 0 1 2 3 4 5 6
	do
		./test --ip 172.20.36.138 -s 30 -v -t 1 -i eth2 -d $delay -l $losses -C yiping -r leopard.html
	done
done

