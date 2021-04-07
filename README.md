DTU1 Simulator
=================
This is a simulator for DTU1 device.

The simulator will generate message and send it to TCP server

Forward tcp server message to a virtual com port using socat:
```
socat -d -d  tcp-listen:8088 pty,link=/tmp/COM2,raw,echo=0
``` 