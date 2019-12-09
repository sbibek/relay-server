#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <thread>
#include "tcp_connection.h"

using namespace std;
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

        tcp_client(boost::asio::io_service& io_service):_socket(io_service) {
            tcp::resolver resolver(io_service);
            boost::asio::async_connect(_socket, resolver.resolve({"localhost","2000"}),
            [this](boost::system::error_code ec, tcp::resolver::iterator){
                if(!ec){
                    cout << "connected"<<std::flush;
                    read();
                } else {
                    cout <<"cannot connect"<<ec<<std::flush;
                }
            });
        }

        static cpointer create(boost::asio::io_service& io_service){
			return cpointer(new tcp_client(io_service));
		}

        void setServer(boost::shared_ptr<tcp_connection> server){

        }

        void connect(){
        }

       void read(){
            auto self(shared_from_this());
			_socket.async_read_some(boost::asio::buffer(&data,1024),
			    [this, self](boost::system::error_code ec,std::size_t len){
				if(!ec && len > 0){
					std::cout << "# "<<len<<data<<std::endl<<std::flush;
                    _server->write(data, len);
				} 
			});
		}

		
		void write(char* message, size_t _len){
            auto self(shared_from_this());
			boost::asio::async_write(_socket,boost::asio::buffer(message,_len),
			 [this, self](boost::system::error_code ec,std::size_t len){
			 	cout << "sent data"<<std::endl<<std::flush;
			 	read();
			});
		}	

		tcp::socket& socket(){
			return _socket;
		}
};