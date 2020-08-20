#!/usr/bin/python
import os 
import sys	

zhuYinList = ['ㄅ','ㄆ','ㄇ','ㄈ','ㄉ','ㄊ','ㄋ','ㄌ','ㄍ','ㄎ','ㄏ','ㄐ','ㄑ','ㄒ','ㄓ','ㄔ','ㄕ','ㄖ','ㄗ','ㄘ','ㄙ','ㄧ','ㄨ','ㄩ','ㄚ','ㄛ','ㄜ','ㄝ','ㄞ','ㄟ','ㄠ','ㄡ','ㄢ','ㄣ','ㄤ','ㄥ','ㄦ']
f2 = open(sys.argv[2],'w',encoding="cp950")
for zhuYin in zhuYinList:
	f1 = open(sys.argv[1],'r',encoding="cp950")
	aa =[]
	for line in f1.readlines():
	    add=0
	    already=0
	    ne = 0
	    first = 0
	    name=''
	    for a in line:
	    	if first == 0 :
	        	first = 1
	        	name = a
	    	if ne== 1 and a == zhuYin and already == 0:
	        	aa.append(name)
	        	already = 1
	        	ne=0
	    	if a == " " or a == "/" :
	        	ne = 1
	    	else :
	        	ne=0
	f2.write(zhuYin+' ')
	for a in aa:
		f2.write(' '+a)
	f2.write('\n')
	for a in aa:
		f2.write(a+'  '+a)
		f2.write('\n')
