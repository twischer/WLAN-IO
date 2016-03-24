#! /bin/bash
# Usage:
# $ ./artNetSend.sh | netcat -u 10.42.0.95 6454
#
while [ 1 ]
do
	echo -e -n "Art-Net\x00\x00\x50\x00\x0E\x58\x00\x01\x00\x00\x01\x00"
	sleep 0.1
done
