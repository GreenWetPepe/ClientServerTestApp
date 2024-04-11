#ifndef CLIENSERVERECN_COMMON_HPP
#define CLIENSERVERECN_COMMON_HPP

#include <string>

static short port = 5555;

namespace Requests
{
    const std::string REGISTRATION = "Reg";
    const std::string LOG_IN = "Log in";
    const std::string PROFILE = "Profile";
    const std::string DEAL = "Deal";
    const std::string CHECK_LOGIN = "Check login";
    const std::string VIEW_DEALS = "View deals";
    const std::string VIEW_TRANSACTIONS = "View transactions";
    const std::string VIEW_PERSONAL_DEALS = "View personal deals";
    const std::string CHECK_NOTIFICATION = "Check notification";
}

namespace DealType
{
    const int BUY = 0;
    const int SELL = 1;
}

namespace ClientStates
{
    const int NOT_REGISTERED = 0;
    const int MAIN = 1;
    const int DEALS_EDITING = 2;
}

namespace ServerResponses
{
    const std::string UNKNOWN_COMMAND = "unknown command";
    const std::string LOGIN_FREE = "correct login"; 
    const std::string LOGIN_TAKEN = "invalid login";
    const std::string INVALID_PASSWORD = "invalid password";
    const std::string LOG_IN_COMPLETE = "log in comlepte";
    const std::string EMPTY_STRING = "empty string";
}


#endif //CLIENSERVERECN_COMMON_HPP
