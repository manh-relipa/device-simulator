DTU1 Simulator
=================
This is a simulator for DTU1 device.

The simulator will generate message and send it to TCP server

Forward tcp server message to a virtual com port using socat:

Build
=================
Using Qt5.13.2 to Build and Run

1. need to install Qt5.13.2:
   Example: ${HOME}/Qt/5.13.2
```  
mkdir build && cd build
``` 
2. run cmd
```  
${HOME}/Qt/Tools/CMake/bin/cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=${HOME}/Qt/5.13.2/gcc_64 
``` 
3. run make to build
```  
make -j4
```

How to use
=================
1. open terminal and run cmd
```
socat -d -d  tcp-listen:8888 pty,link=/tmp/COM2,raw,echo=0
``` 
if you don't have socat please install it:
```
sudo apt-get install socat
``` 
2. go to the build directory and run
```  
./DeviceSimulator
```
3. you can change the tcp-listen from file settings.ini in the build directory