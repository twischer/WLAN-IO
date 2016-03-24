#!/bin/bash
hostname=192.168.4.1

curl -m 10 -s "http://$hostname/log/reset"
curl -m 10 -s "http://$hostname/log/reset"
