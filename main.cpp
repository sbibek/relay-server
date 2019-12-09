#include <iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>
#include <thread>

using namespace std;
using namespace boost::asio;
using boost::asio::ip::tcp;

class tcp_connection;

class tcp_client : public boost::enable_shared_from_this<tcp_client>
{
private:
    tcp::socket _socket;
    std::string _message;
    char data[1024];
    tcp_connection *_server;

public:
    typedef boost::shared_ptr<tcp_client> cpointer;

    tcp_client(boost::asio::io_service &io_service, tcp_connection *srv);

    static cpointer create(boost::asio::io_service &io_service, tcp_connection *srv)
    {
        return cpointer(new tcp_client(io_service, srv));
    }
    void read();
    void write(char *message, size_t _len);
    tcp::socket &socket();
};

class tcp_connection : public boost::enable_shared_from_this<tcp_connection>
{
private:
    tcp::socket _socket;
    std::string _message;
    tcp_client::cpointer client;
    char data[1024];

public:
    typedef boost::shared_ptr<tcp_connection> pointer;

    static pointer create(boost::asio::io_service &io_service)
    {
        return pointer(new tcp_connection(io_service));
    }

    void start();

    void read();

    boost::shared_ptr<tcp_connection> getpointer();

    void write(char *message, size_t _len);

    tcp::socket &socket();

private:
    tcp_connection(boost::asio::io_service &io_service);
};

inline tcp_client::tcp_client(boost::asio::io_service &io_service, tcp_connection *srv) : _socket(io_service), _server(srv)
{
    tcp::resolver resolver(io_service);
    boost::asio::async_connect(_socket, resolver.resolve({"localhost", "20000"}),
                               [this](boost::system::error_code ec, tcp::resolver::iterator) {
                                   if (!ec)
                                   {
                                       cout << "connected" << std::flush;
                                       read();
                                   }
                                   else
                                   {
                                       cout << "cannot connect" << ec << std::flush;
                                   }
                               });
}

inline void tcp_client::read()
{
    auto self(shared_from_this());
    _socket.async_read_some(boost::asio::buffer(&data, 1024),
                            [this, self](boost::system::error_code ec, std::size_t len) {
                                if (!ec && len > 0)
                                {
                                    std::cout << "outstation -> master " << len << "bytes" << std::endl
                                              << std::flush;
                                    try
                                    {
                                        _server->getpointer()->write(data, len);
                                    }
                                    catch (std::exception e)
                                    {
                                        cout << e.what();
                                    }
                                    read();
                                }
                            });
}

inline void tcp_client::write(char *message, size_t _len)
{
    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(message, _len),
                             [this, self](boost::system::error_code ec, std::size_t len) {
                                 read();
                             });
}

inline tcp::socket &tcp_client::socket()
{
    return _socket;
}

inline tcp_connection::tcp_connection(boost::asio::io_service &io_service) : _socket(io_service)
{
}

inline void tcp_connection::start()
{
    cout << "started" << std::flush;
    client = tcp_client::create(_socket.get_io_service(), this);
    read();
}
inline void tcp_connection::read()
{
    auto self(shared_from_this());
    _socket.async_read_some(boost::asio::buffer(&data, 1024),
                            [this, self](boost::system::error_code ec, std::size_t len) {
                                if (!ec && len > 0)
                                {
                                    std::cout << "master -> outstation " << len << "bytes" << std::endl
                                              << std::flush;
                                    client->write(data, len);
                                    read();
                                }
                            });
}

inline void tcp_connection::write(char *message, size_t _len)
{
    auto self(shared_from_this());
    boost::asio::async_write(_socket, boost::asio::buffer(message, _len),
                             [this, self](boost::system::error_code ec, std::size_t len) {
                                 read();
                             });
}

inline boost::shared_ptr<tcp_connection> tcp_connection::getpointer()
{
    return this->shared_from_this();
}

inline tcp::socket &tcp_connection::socket()
{
    return _socket;
}

class tcp_server
{
private:
    tcp::acceptor _acceptor;

    void start_accept()
    {
        cout << "server ready to accept connections" << std::endl
             << std::flush;
        tcp_connection::pointer new_connection = tcp_connection::create(_acceptor.get_io_service());
        _acceptor.async_accept(new_connection->socket(), boost::bind(&tcp_server::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }

    void handle_accept(tcp_connection::pointer new_connection, const boost::system::error_code &error)
    {
        if (!error)
        {
            new_connection->start();
        }

        start_accept();
    }

public:
    tcp_server(boost::asio::io_service &io_service) : _acceptor(io_service, tcp::endpoint(tcp::v4(), 2000))
    {
        start_accept();
    }
};

int main()
{
    try
    {

        boost::asio::io_service io_service;
        tcp_server server(io_service);
        io_service.run();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
