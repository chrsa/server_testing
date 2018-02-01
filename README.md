# server_testing

File dependencies:
ftp://ftp.bmf.com.br/MarketData/BMF/OFER_CPA_BMF_20180126.zip
ftp://ftp.bmf.com.br/MarketData/BMF/OFER_CPA_BMF_20180126.zip

Library dependency:
Boost Asio

# step1.py

step1.py is a Python3 script that analysis the CPA and VDA files in order to know which is the most liquid instrument.

Usage:
Put step1.py + OFER_CPA_BMF_20180126.TXT + OFER_VDA_BMF_20180126.TXT in the same folder.
All that you need to do is:
$ chmod +x step1.py
$ ./step1.py

The output is "WING18".

# step2.cpp

step2.cpp is a C++/BoostAsio server that takes two command-line options: 
1- The port to which it has to listen
2- The instrument that the server has to filter from VDA and CPA files

Usage:
Put step2.cpp + OFER_CPA_BMF_20180126.TXT + OFER_VDA_BMF_20180126.TXT in the same folder.
$ CC step2.cpp -O3 -std=c++14 -lboost_system -lpthread -L/usr/local/lib -I/usr/local/include -o server
$ su
% ./server 100 WING18
The above command should start the reading from the VDA and CPA files to memory. When the server is Ready to work in the network, it will print to terminal:
"Ready."
You must wait for "Ready." before connect to server.

The protocol is:
HH:MM:SS-HH:MM:SS\n
where \n means "new line" instead literal '\' and 'n' 
and HH=hour,MM=minute,SS=second
Example
10:10:0-10:11:20

The server sends all market-orders inside that interval, VDAs and CPAs, sorted by time.

You can test the server using netcat:
$ nc 192.168.0.101 100
and writing 10:10:0-10:11:20 and pressing Enter.
(Assuming server running in the IP 192.168.0.101 and port 100)

Details:
The server is iterative and syncronous, that is, it can not accept more than one connection by time. 

# step3.cpp step3.hpp

step3.cpp/step3.hpp is a library to facilitate the communication with the server.
It is very simple to use. 
Example:

    // initializing client
    Client c("192.168.0.101", 100);
    
    // Connect to server in order to receive data from 10:10:00 to 10:20:00
    c << pair<Time, Time>{ {10, 10,0}, {10,20,0} };

    // member function buffer() returns the internal data, a vector<char>
    // So, printing the data:
    for(auto &x : c.buffer() )
           cout << x;
    
    // And cleaning the internal vector<char>
    c.clear();
    
The main.cpp has a similar example of use.
    
