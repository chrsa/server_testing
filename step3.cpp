#include "step3.hpp"

Client &Client::operator<<(const pair<Time, Time> &p)
{
  	try
  	{
    		boost::asio::io_context io_context;
		boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(host), port );
    		tcp::socket socket(io_context);
		socket.connect(endpoint);

		socket.write_some(boost::asio::buffer(	to_string(p.first.h) + ":" + to_string(p.first.m) + ":"+ to_string(p.first.s) + "-"+
								to_string(p.second.h) + ":" +to_string(p.second.m)+ ":"+ to_string(p.second.s) + "\n"));

    		for (;;)
    		{
			std::vector<char> buf(128);
			boost::system::error_code error;

      			socket.read_some(boost::asio::buffer(buf), error);

      			if (error == boost::asio::error::eof)
				break; // Connection closed cleanly by peer.
      			else if (error)
				throw boost::system::system_error(error); // Some other error.

      			vbuffer.insert(vbuffer.end(), buf.begin(), buf.end());
    		}

  	}
  	catch (std::exception& e)
  	{
    		std::cerr << e.what() << std::endl;
  	}

	return *this;

}

