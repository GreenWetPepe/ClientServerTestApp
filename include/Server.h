#include <iostream>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "Session.h"

class Server
{
public:
    Server(boost::asio::io_service& io_service);

    void handle_accept(Session* new_session,
        const boost::system::error_code& error);

private:
    boost::asio::io_service& io_service_;
    tcp::acceptor acceptor_;
};