#include<iostream>
#include<boost/asio.hpp>

using namespace std;
using namespace boost::asio;

int main() {
    cout << "here";
    io_service io_service;
    ip::tcp::acceptor acceptor(io_service, ip::tcp::endpoint(ip::tcp::v4(), 2001));
    for(;;){
        cout << "somebody connected";
        ip::tcp::socket socket(io_service);
        acceptor.accept(socket);
        boost::system::error_code ie;
        write(socket, boost::asio::buffer("test"), ie);
        std::flush(std::cout);
    }

    return 0;
}