#!/bin/bash

for delay in 100
do
	for losses in 0
	do
		./test --ip 172.20.36.138 -s 30 -t 1 -i eth0 -d $delay -l $losses -C yiping -r leopard_better.html 
		# alexa/amazon/index.html alexa/baidu/index.html alexa/bing/index.html alexa/ebay/index.html alexa/google/index.html alexa/hao123/index.html alexa/qq/index.html alexa/sohu/index.html alexa/taobao/index.html alexa/tmall/index.html alexa/weibo/index.html alexa/wikipedia/index.html alexa/yahoo/index.html alexa/yahoo.jp/index.html alexa/yandex/index.html alexa/youtube/index.html
	done
done

