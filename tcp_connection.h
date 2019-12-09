#ifndef __tcp_c__
#define __tcp_c__

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "tcp_client.h"

class tcp_client;

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>{
	private:
		tcp::socket _socket;
		std::string _message;
        boost::shared_ptr<tcp_client> client;
		char data[1024];
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;

		static pointer create(boost::asio::io_service& io_service);
		void start();
		void read();
		
		void write(char* message, size_t _len);
		tcp::socket& socket();
	private:
		tcp_connection(boost::asio::io_service& io_service);
		void handle_write(const boost::system::error_code&, size_t);
};

#endif