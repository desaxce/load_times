#!/bin/bash

for delay in 100
do
	for losses in 0 6
	do
		#./test --ip youtube.com -s 30 -t 30 -i eth0 -d $delay -l $losses -C checkyoutube -r /
		#sina was removed because it made h2o bug
		./test --ip 172.20.36.138 -s 30 -t 5 -i eth0 -d $delay -l $losses -C losses -r leopard.html amazon/index.html baidu/index.html bing/index.html ebay/index.html google/index.html hao123/index.html qq/index.html sohu/index.html taobao/index.html tmall/index.html weibo/index.html wikipedia/index.html yahoo/index.html yandex/index.html youtube/index.html
		#./test --ip youtube.com -s 60 -t 10 -i eth0 -d $delay -l $losses -C checkyoutube -r /
		#./test --ip google.com -s 60 -t 10 -i eth0 -d $delay -l $losses -C checkyoutube -r /
		#./test --ip nghttp2.org -s 60 -t 10 -i eth0 -d $delay -l $losses -C checkyoutube -r /
		#./test --ip 172.20.36.138 -s 45 -t 2 -i eth0 -d $delay -l $losses -C requests -r nb_request/1.html nb_request/2.html nb_request/3.html nb_request/4.html nb_request/5.html nb_request/6.html nb_request/7.html nb_request/8.html nb_request/9.html nb_request/10.html nb_request/11.html nb_request/12.html nb_request/13.html nb_request/14.html nb_request/15.html nb_request/16.html nb_request/17.html nb_request/18.html nb_request/19.html nb_request/20.html nb_request/21.html nb_request/22.html nb_request/23.html nb_request/24.html nb_request/25.html nb_request/26.html nb_request/27.html nb_request/28.html nb_request/29.html nb_request/30.html nb_request/31.html nb_request/32.html nb_request/33.html nb_request/34.html nb_request/35.html nb_request/36.html nb_request/37.html nb_request/38.html nb_request/39.html nb_request/40.html nb_request/41.html nb_request/42.html nb_request/43.html nb_request/44.html nb_request/45.html nb_request/46.html nb_request/47.html nb_request/48.html nb_request/49.html nb_request/50.html nb_request/51.html nb_request/52.html nb_request/53.html nb_request/54.html nb_request/55.html nb_request/56.html nb_request/57.html nb_request/58.html nb_request/59.html nb_request/60.html nb_request/61.html nb_request/62.html nb_request/63.html nb_request/64.html nb_request/65.html nb_request/66.html nb_request/67.html nb_request/68.html nb_request/69.html nb_request/70.html nb_request/71.html nb_request/72.html nb_request/73.html nb_request/74.html nb_request/75.html nb_request/76.html nb_request/77.html nb_request/78.html nb_request/79.html nb_request/80.html nb_request/81.html nb_request/82.html nb_request/83.html nb_request/84.html nb_request/85.html nb_request/86.html nb_request/87.html nb_request/88.html nb_request/89.html nb_request/90.html nb_request/91.html nb_request/92.html nb_request/93.html nb_request/94.html nb_request/95.html nb_request/96.html nb_request/97.html nb_request/98.html nb_request/99.html nb_request/100.html
	done
done

