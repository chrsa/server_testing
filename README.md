# server_testing

File dependencies: <br />
ftp://ftp.bmf.com.br/MarketData/BMF/OFER_CPA_BMF_20180126.zip <br />
ftp://ftp.bmf.com.br/MarketData/BMF/OFER_CPA_BMF_20180126.zip <br />

Library dependency: <br />
Boost Asio 1.66.0 <br />

# step1.py

step1.py is a Python3 script that analysis the CPA and VDA files in order to know which is the most liquid instrument. <br />

Usage: <br />
Put step1.py + OFER_CPA_BMF_20180126.TXT + OFER_VDA_BMF_20180126.TXT in the same folder. <br />
All that you need to do is: <br />
$ chmod +x step1.py <br />
$ ./step1.py <br />

The output is "WING18". <br />

# step2.cpp

step2.cpp is a C++/BoostAsio server that takes two command-line options:  <br />
1- The port to which it has to listen <br />
2- The instrument that the server has to filter from VDA and CPA files <br />

Usage: <br />
Put step2.cpp + OFER_CPA_BMF_20180126.TXT + OFER_VDA_BMF_20180126.TXT in the same folder. <br />
$ CC step2.cpp -O3 -std=c++14 -lboost_system -lpthread -L/usr/local/lib -I/usr/local/include -o server <br />
$ su <br />
% ./server 100 WING18 <br />
The above command should start the reading from the VDA and CPA files to memory. When the server is Ready to work in the network, it will print to terminal: <br />
"Ready." <br />
You must wait for "Ready." before connect to server. <br />

The protocol is: <br />
HH:MM:SS-HH:MM:SS\n <br />
where \n means "new line" instead literal '\' and 'n'  <br />
and HH=hour,MM=minute,SS=second <br />
Example <br />
10:10:0-10:11:20 <br />

The server sends all market-orders inside that interval, VDAs and CPAs, sorted by time. <br />

You can test the server using netcat: <br />
$ nc 192.168.0.101 100 <br />
and writing 10:10:0-10:11:20 and pressing Enter. <br />
(Assuming server running in the IP 192.168.0.101 and port 100) <br />

Details: <br />
The server is iterative and syncronous, that is, it can not accept more than one connection by time.  <br />

# step3.cpp step3.hpp <br />

step3.cpp/step3.hpp is a library to facilitate the communication with the server. <br />
It is very simple to use.  <br />
Example: <br />

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
    
The main.cpp has a similar example of use. <br />
    
