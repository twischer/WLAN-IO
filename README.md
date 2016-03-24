# WLAN-RGB-Para
The ESP8266 is used as an Art-Net-Node and the 3 output pins will be used as PWM outputs. So a RGB-LED could be directly connected and controlled over WLAN.
The inital version was copied from http://www.ulrichradig.de/home/index.php/dmx/wlan-art-net-node.

# Energy consumtion
Supply	Off				On
4,8V	75mA (360mW)
8V		75mA (600mW)	3,40A (27,2W)
10V						2,58A
12V						2,03A
14V						1,68A
15V		75mA (1,13W)	1,57A (23,6W)


# HOWTO test mosquitto (MQTT broker)
    $ sudo apt-get install mosquitto mosquitto-clients

    $ mosquitto_pub -h [HOST] -m hello -t test -r

    $ mosquitto_sub -h [HOST] -t test -d
