#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>

#include "json.hpp"
#include "Common.hpp"
#include "Core.h"
#include "Session.h"

using boost::asio::ip::tcp;

Core& GetCore()
{
    static Core core;
    return core;
}

Session::Session(boost::asio::io_service& io_service)
    : socket_(io_service)
{
}

tcp::socket& Session::socket()
{
    return socket_;
}

void Session::start()
{
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        boost::bind(&Session::handle_read, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

// Обработка полученного сообщения.
void Session::handle_read(const boost::system::error_code& error,
    size_t bytes_transferred)
{
    if (!error)
    {
        data_[bytes_transferred] = '\0';

        // Парсим json, который пришёл нам в сообщении.
        auto j = nlohmann::json::parse(data_);
        auto reqType = j["ReqType"];

        std::string reply = ServerResponses::UNKNOWN_COMMAND;
        if (reqType == Requests::REGISTRATION)
        {
            reply = GetCore().RegisterNewUser(j["Message"]);
        }
        else if (reqType == Requests::LOG_IN)
        {
            reply = GetCore().LogInUser(j["UserLogin"], j["Message"]);
        }
        else if (reqType == Requests::DEAL)
        {
            reply = GetCore().AddDeal(j["UserLogin"], j["Message"]) + "\n";
            GetCore().ProcessLastDeal();
        }
        else if (reqType == Requests::PROFILE)
        {
            reply = GetCore().GetUserData(j["UserLogin"]);
        }
        else if (reqType == Requests::CHECK_LOGIN)
        {
            reply = GetCore().CheckLoginAvailability(j["Message"]);
        }
        else if (reqType == Requests::VIEW_DEALS)
        {
            reply = GetCore().GetDealsAsString("");
        }
        else if (reqType == Requests::VIEW_TRANSACTIONS)
        {
            reply = GetCore().GetTransactionsString();
        }
        else if (reqType == Requests::VIEW_PERSONAL_DEALS)
        {
            reply = GetCore().GetDealsAsString(j["UserLogin"]);
        }
        else if (reqType == Requests::CHECK_NOTIFICATION)
        {
            reply = GetCore().GetNotifications(j["UserLogin"]);
        }

        if (reply.empty()) reply = ServerResponses::EMPTY_STRING;

        boost::asio::async_write(socket_,
            boost::asio::buffer(reply, reply.size()),
            boost::bind(&Session::handle_write, this,
                boost::asio::placeholders::error));
    }
    else
    {
        delete this;
    }
}

void Session::handle_write(const boost::system::error_code& error)
{
    if (!error)
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
            boost::bind(&Session::handle_read, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
    }
    else
    {
        delete this;
    }
}