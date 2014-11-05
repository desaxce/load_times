#!/bin/bash

for delay in 0 100
do
	for losses in 0 5
	do
		# sina was removed because it made h2o bug
		./test --ip 172.20.36.138 -s 30 -t 5 -i eth0 -d $delay -l $losses -C yiping -r leopard_better.html amazon/index.html baidu/index.html bing/index.html ebay/index.html google/index.html hao123/index.html qq/index.html sohu/index.html taobao/index.html tmall/index.html weibo/index.html wikipedia/index.html yahoo/index.html yahoo.jp/index.html yandex/index.html youtube/index.html
	done
done

