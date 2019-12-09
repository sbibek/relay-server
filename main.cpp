#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>{
	private:
		tcp::socket _socket;
		std::string _message;
		char data[1024];
	public:
		typedef boost::shared_ptr<tcp_connection> pointer;

		static pointer create(boost::asio::io_service& io_service){
			return pointer(new tcp_connection(io_service));
		}

		void start(){
			cout << "started"<<std::flush;
			read();
		}

		void read(){
			auto self(shared_from_this());
			_socket.async_read_some(boost::asio::buffer(&data,1024),
			[this,self](boost::system::error_code ec,std::size_t len){
				if(!ec && len > 0){
					std::cout << "# "<<len<<data<<std::endl<<std::flush;
					write(data,len);
				} 
			});
		}

		
		void write(char* message, size_t _len){
			auto self(shared_from_this());
			boost::asio::async_write(_socket,boost::asio::buffer(message,_len),
			 [this,self](boost::system::error_code ec,std::size_t len){
			 	cout << "sent data"<<std::endl<<std::flush;
			 	read();
			});
		}	

		tcp::socket& socket(){
			return _socket;
		}

	private:
		tcp_connection(boost::asio::io_service& io_service): _socket(io_service){
		}

		void handle_write(const boost::system::error_code&, size_t){
		}
};

class tcp_server{
	private:
		tcp::acceptor _acceptor;	

		void start_accept(){
			cout << "server ready to accept connections"<<std::endl<<std::flush;
			tcp_connection::pointer new_connection = tcp_connection::create(_acceptor.get_io_service());
			_acceptor.async_accept(new_connection->socket(),boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
		}

		void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code& error){
			if(!error){
				new_connection->start();
			}

			start_accept();
		}

	public:
		tcp_server(boost::asio::io_service& io_service): _acceptor(io_service, tcp::endpoint(tcp::v4(),2001)){
			start_accept();
		}
};

int main() {
	try{

		boost::asio::io_service io_service;
		tcp_server server(io_service);
		io_service.run();

	} catch(std::exception & e){
		std::cerr << e.what() << std::endl;
	}
	return 0;
}
