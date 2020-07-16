#!/usr/bin/python
#coding=UTF-8
 
"""
TCP Client sample
"""
 
import socket
 
target_host = "140.122.185.98"
target_port = 12000
 
# create socket
# AF_INET 代表使用標準 IPv4 位址或主機名稱
# SOCK_STREAM 代表這會是一個 TCP client
client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
 
# client 建立連線
client.connect((target_host, target_port))
 
# 傳送資料給 target
client.send("GET / HTTP/1.1\r\nHost: google.com\r\n\r\n")
 
# 接收資料
response = client.recv(4096)
 
# 印出資料信息
print response
