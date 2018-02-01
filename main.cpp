#include "step3.hpp"

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cerr << "Usage: client <host> <port>" << endl;
		return 1;
	}
        Client c(argv[1], stoi(argv[2]));

        cout << "Receiving data..." << endl;

        c << pair<Time, Time>{ {10, 10,0}, {10,20,0} };

        for(auto &x : c.buffer() )
                cout << x;

        cout << "Cleaning now.\n" << endl;
        c.clear();

        cout << "Is it empty?" << endl;
        for(auto &x : c.buffer() )
                cout << x;

        cout << "...yes, it is." << endl;

        return 0;
}

