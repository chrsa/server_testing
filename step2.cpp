#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <set>
#include <initializer_list>
#include <boost/asio.hpp>
#include <algorithm>

using namespace std;
using boost::asio::ip::tcp;

// ---------------------------------------------------------------
// constant information from specification 
// ftp://ftp.bmf.com.br/MarketData/OFER_CPA_LAYOUT_english.txt

enum class POSITION
{
	// header/trailer
	IDENTIFICATION = 0,
	NAME_OF_FILE = 3,
	INITIAL_DATE = 23,
	END_DATE = 34,
	TOTAL_OF_LINES = 45, 
	
	// details 
	SESSION_DATE = 0,
	INSTRUMENT_SYMBOL = 11,
	ORDER_SIDE = 62,
	SEQUENTIAL_ORDER_NUMBER = 64,
	SECONDARY_ORDER_ID = 80,
	EXECUTION_TYPE = 96,
	PRIORITY_TIME = 100,
	PRIORITY_INDICATOR = 116,
	ORDER_PRICE = 127,
	TOTAL_QUANTITY_OF_ORDER = 148,
	TRADED_QUANTITITY_OF_ORDER = 167,
	ORDER_DATE = 186,
	ORDER_DATETIME_ENTRY = 197,
	ORDER_STATUS = 217,
	AGRESSOR_INDICATOR = 219,
	MEMBER = 221
}; 


enum class LENGTH
{
	// header/trailer
	IDENTIFICATION = 2,
	NAME_OF_FILE = 20,
	INITIAL_DATE = 10,
	END_DATE = 10,
	TOTAL_OF_LINES = 9, 
	
	// details 
	SESSION_DATE = 10,
	INSTRUMENT_SYMBOL = 50,
	ORDER_SIDE = 1,
	SEQUENTIAL_ORDER_NUMBER = 15,
	SECONDARY_ORDER_ID = 15,
	EXECUTION_TYPE = 3,
	PRIORITY_TIME = 15,
	PRIORITY_INDICATOR = 10,
	ORDER_PRICE = 20,
	TOTAL_QUANTITY_OF_ORDER = 18,
	TRADED_QUANTITITY_OF_ORDER = 18,
	ORDER_DATE = 10,
	ORDER_DATETIME_ENTRY = 19,
	ORDER_STATUS = 1,
	AGRESSOR_INDICATOR = 1,
	MEMBER = 8
}; 

// -----------------------------------------------------------

/* 
 * The input has the following format:
 * HEADER
 * ORDER
 * ORDER
 * ORDER
 * ...
 * ORDER
 * TRAILER 
 */

// Rh_Rt can be header (rh) or trailer (rt)
struct Rh_Rt
{
	string identification;   
	string name_of_file;
	int initial_date[3];
	int end_date[3];
	size_t total_of_lines;
};

struct Order
{
	int session_date[3];
	string instrument_symbol;
	int order_side;
	long sequential_order_number;
	long secondary_order_id;
	int execution_type;
	int priority_time[4];
	long priority_indicator;
	int order_price[2];
	int total_quantity_of_order;
	int traded_quantity_of_order;
	int order_date[3];
	int order_datetime_entry[6];
	int order_status;
	int agressor_indicator;
	int member;
};

struct Order_sort
{
	bool operator()(const Order &x, const Order &y) 
	{
		return (x.priority_time[0]*60l*60l*1000000l+x.priority_time[1]*60l*1000000l+x.priority_time[2]*1000000l + x.priority_time[3] < y.priority_time[0]*60l*60l*1000000l+y.priority_time[1]*60l*1000000l+y.priority_time[2]*1000000l + y.priority_time[3]);
	}
};


// ------------------------------------------------------------
istream &operator>>(istream &in, Rh_Rt &x)
{
	string s, test;
	char c;

	getline(in, s);
	
	if( (test = s.substr(static_cast<size_t>(POSITION::IDENTIFICATION), static_cast<size_t>(LENGTH::IDENTIFICATION) )) != "RH" && test != "RT")
	{
		in.clear(in.rdstate() | ios::failbit);  
		return in;
	}

	x.identification = test;

	x.name_of_file = s.substr(static_cast<size_t>(POSITION::NAME_OF_FILE), static_cast<size_t>(LENGTH::NAME_OF_FILE));
	
	istringstream iss{ s.substr(static_cast<size_t>(POSITION::INITIAL_DATE), static_cast<size_t>(LENGTH::INITIAL_DATE)) };
	iss >>  x.initial_date[0] >> c >> x.initial_date[1] >> c >> x.initial_date[2];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::END_DATE), static_cast<size_t>(LENGTH::END_DATE)) );
	iss >>  x.end_date[0] >> c >> x.end_date[1] >> c >> x.end_date[2];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::TOTAL_OF_LINES), static_cast<size_t>(LENGTH::TOTAL_OF_LINES)) );
	iss >> x.total_of_lines;
	iss.clear();

	return in;
}

ostream &operator<<(ostream &os, const Rh_Rt &x)
{
	os << "Identification: " << x.identification << endl;
	os << "Name of file: " << x.name_of_file << endl;
	os << "Initial date: " << x.initial_date[0] << "-" << x.initial_date[1] << "-" << x.initial_date[2] << endl;
	os << "End date: " << x.end_date[0] << "-" << x.end_date[1] << "-" << x.end_date[2] << endl;
	os << "Total of lines: " << x.total_of_lines << endl;

	return os;
}


istream& operator>>(istream &in, Order &x)
{
	string s, test;
	char c;

	getline(in, s);
	
	if( (test = s.substr(static_cast<size_t>(POSITION::IDENTIFICATION), static_cast<size_t>(LENGTH::IDENTIFICATION) )) == "RH" || test == "RT")
	{
		in.clear(in.rdstate() | ios::failbit);  
		return in;
	}

	istringstream iss{ s.substr(static_cast<size_t>(POSITION::SESSION_DATE), static_cast<size_t>(LENGTH::SESSION_DATE)) };
	iss >>  x.session_date[0] >> c >> x.session_date[1] >> c >> x.session_date[2];
	iss.clear();

	x.instrument_symbol = s.substr(static_cast<size_t>(POSITION::INSTRUMENT_SYMBOL), static_cast<size_t>(LENGTH::INSTRUMENT_SYMBOL));

	iss.str( s.substr(static_cast<size_t>(POSITION::ORDER_SIDE), static_cast<size_t>(LENGTH::ORDER_SIDE)) );
	iss >>  x.order_side;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::SEQUENTIAL_ORDER_NUMBER), static_cast<size_t>(LENGTH::SEQUENTIAL_ORDER_NUMBER)) );
	iss >>  x.sequential_order_number;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::SECONDARY_ORDER_ID), static_cast<size_t>(LENGTH::SECONDARY_ORDER_ID)) );
	iss >>  x.secondary_order_id;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::EXECUTION_TYPE), static_cast<size_t>(LENGTH::EXECUTION_TYPE)) );
	iss >>  x.execution_type;
	iss.clear();
	
	iss.str( s.substr(static_cast<size_t>(POSITION::PRIORITY_TIME), static_cast<size_t>(LENGTH::PRIORITY_TIME)) );
	iss >>  x.priority_time[0] >> c >> x.priority_time[1] >> c >> x.priority_time[2] >> c >> x.priority_time[3];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::PRIORITY_INDICATOR), static_cast<size_t>(LENGTH::PRIORITY_INDICATOR)) );
	iss >>  x.priority_indicator;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::ORDER_PRICE), static_cast<size_t>(LENGTH::ORDER_PRICE)) );
	iss >>  x.order_price[0] >> c >> x.order_price[1];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::TOTAL_QUANTITY_OF_ORDER), static_cast<size_t>(LENGTH::TOTAL_QUANTITY_OF_ORDER)) );
	iss >>  x.total_quantity_of_order;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::TRADED_QUANTITITY_OF_ORDER), static_cast<size_t>(LENGTH::TRADED_QUANTITITY_OF_ORDER)) );
	iss >>  x.traded_quantity_of_order;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::ORDER_DATE), static_cast<size_t>(LENGTH::ORDER_DATE)) );
	iss >>  x.order_date[0] >> c >> x.order_date[1] >> c >> x.order_date[2];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::ORDER_DATETIME_ENTRY), static_cast<size_t>(LENGTH::ORDER_DATETIME_ENTRY)) );
	iss >>  x.order_datetime_entry[0] >> c >> x.order_datetime_entry[1] >> c >> x.order_datetime_entry[2] >>  x.order_datetime_entry[3] >> c >> x.order_datetime_entry[4] >> c >> x.order_datetime_entry[5];
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::ORDER_STATUS), static_cast<size_t>(LENGTH::ORDER_STATUS)) );
	iss >>  x.order_status;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::AGRESSOR_INDICATOR), static_cast<size_t>(LENGTH::AGRESSOR_INDICATOR)) );
	iss >>  x.agressor_indicator;
	iss.clear();

	iss.str( s.substr(static_cast<size_t>(POSITION::MEMBER), static_cast<size_t>(LENGTH::MEMBER)) );
	iss >>  x.member;
	iss.clear();


	return in;
}

ostream &operator<<(ostream &os, const Order &x)
{
	os << "Session date: " << x.session_date[0] << "-" << x.session_date[1] << "-" << x.session_date[2] << endl;
	os << "Instrument symbol: " << x.instrument_symbol << endl;
	os << "Order side: " << x.order_side << endl;
	os << "Sequential order number: " << x.sequential_order_number << endl;
	os << "Secondary order ID: " << x.secondary_order_id << endl;
	os << "Execution type: " << x.execution_type << endl;
	os << "Priority time: " << x.priority_time[0] << ":" << x.priority_time[1] << ":" <<x.priority_time[2] << "." << x.priority_time[3] << endl;
	os << "Priority indicator: " << x.priority_indicator << endl;
	os << "Order price: " << x.order_price[0] << "." << x.order_price[1] << endl;
	os << "Total quantity of order: " << x.total_quantity_of_order << endl;
	os << "Traded quantity of order: " << x.traded_quantity_of_order << endl;
	os << "Order date: " << x.order_date[0] << "-" << x.order_date[1] << "-" << x.order_date[2] << endl;
	os << "Order datetime entry: " << x.order_datetime_entry[0] << "-" << x.order_datetime_entry[1] << "-" << x.order_datetime_entry[2] << " " << x.order_datetime_entry[3] << ":" << x.order_datetime_entry[4] << ":" << x.order_datetime_entry[5] << endl;
	os << "Order status: " << x.order_status << endl;
	os << "Agressor indicator: " << x.agressor_indicator << endl;
	os << "Member: " << x.member << endl;

	return os;
}


class MarketData 
{
	private:
		string instrument;
		multiset<Order, Order_sort> orders;
	public:
		MarketData(const string &instr);
		void show() const;
		multiset<Order, Order_sort>::iterator iterator_begin() {return orders.begin();}
		multiset<Order, Order_sort>::iterator iterator_end() {return orders.end();}

};

// construct server with intrument specified
MarketData::MarketData(const string &instr): instrument{instr}, orders{}
{
	Rh_Rt header;
	
	for(const string &str : initializer_list<string>{"OFER_VDA_BMF_20180126.TXT", "OFER_CPA_BMF_20180126.TXT"})
	{	
		int state = 0;
		ifstream fi{str};

		fi >> header;
	
		for(Order x; state!=2 && fi >> x; )
		{
			if(x.instrument_symbol == instrument)
			{
				if(state==0)
					state = 1;
				orders.insert(x);	
			}
			else
			{
				if(state==1)
					state = 2;
			}
		}	
	}

}

void MarketData::show() const
{
	for(auto &x : orders)
	{
		cout << x << endl;
	}
}

class Server
{
	private:
		MarketData md;
	public:
		Server(const string &instrument, unsigned port);
};

Server::Server(const string &instrument, unsigned port): md{instrument}
{

        try
        {
                boost::asio::io_context io_context;

                tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), port ) );

                for (;;)
                {
                        tcp::socket socket(io_context);
			cout << "Ready." << endl;
                        acceptor.accept(socket);
                        boost::asio::streambuf buffer;
                        boost::asio::read_until(socket, buffer, "\n");
                        std::istream is(&buffer); 

			Order ord1, ord2;
			char ch[5];
			is >> 	ord1.priority_time[0] >> ch[0] >> ord1.priority_time[1] >> ch[1] >> ord1.priority_time[2] >> ch[2] >>
				ord2.priority_time[0] >> ch[3] >> ord2.priority_time[1] >> ch[4] >> ord2.priority_time[2] ;	
			ord1.priority_time[3] = 0;
			ord2.priority_time[3] = 0;
			if(!is || ch[0]!=':' || ch[1]!=':' || ch[2]!='-' || ch[3]!=':' || ch[4]!=':' || !Order_sort{}(ord1, ord2))
	     			socket.write_some(boost::asio::buffer("Bad input. Correct format = HH:MM:SS-HH:MM:SS + '\\n'. With first time less than second time.\n"));
			else
			{
				auto bound1 = lower_bound(md.iterator_begin(), md.iterator_end(), ord1, Order_sort{});
				auto bound2 = upper_bound(md.iterator_begin(), md.iterator_end(), ord2, Order_sort{});
				for(;bound1 != bound2 ; bound1++)
				{
					ostringstream os{};
					os << *bound1 << endl;
					socket.write_some( boost::asio::buffer(os.str()));
				}
			}


                }
        }
        catch (std::exception& e)
        {
                std::cerr << e.what() << std::endl;
        }
	
}

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cerr << "Usage: server <port> <instrument>" << endl;	
	}

	string instrument_input{argv[2]} ;
	instrument_input += string(static_cast<unsigned>(LENGTH::INSTRUMENT_SYMBOL)-instrument_input.size(), ' ');


	Server serv{ instrument_input, static_cast<unsigned>(stoi(argv[1])) };

	return 0;
}

