#ifndef __tcp_cli__
#define __tcp_cli__

#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include "tcp_connection.h"

class tcp_connection;

using namespace boost::asio;
using boost::asio::ip::tcp;

class tcp_client: public boost::enable_shared_from_this<tcp_client> {
    private:
        tcp::socket _socket;
        std::string _message;
		char data[1024];
        boost::shared_ptr<tcp_connection> _server;
    public:
        typedef boost::shared_ptr<tcp_client> cpointer;

        tcp_client(boost::asio::io_service& io_service);

        static cpointer create(boost::asio::io_service& io_service);

        void setServer(boost::shared_ptr<tcp_connection> server);
        void connect();
        void read();
		void write(char* message, size_t _len);
		tcp::socket& socket();
};

#endif