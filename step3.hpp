#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <sstream>
#include <string>
#include <utility>

using namespace std;
using boost::asio::ip::tcp;

struct Time
{
	int h; // hour
	int m; // minute
	int s; // second
};

class Client
{
	private:
		vector<char> vbuffer;
		string host;
		int port;
		
	public:
		Client(const string &h, const int p) : host{h}, port{p} {} 
	
		Client &operator<<(const pair<Time, Time> &p);

		const vector<char> &buffer()
		{
			return vbuffer;
		}

		void clear()
		{
			vbuffer.clear();
		}

};

