#include "Transaction.h"

Transaction::Transaction(unsigned int purchasedCurrency, unsigned int currencySpent,
            std::string sellerLogin, std::string buyerLogin) :
                purchasedCurrency_(purchasedCurrency),
                currencySpent_(currencySpent),
                sellerLogin_(sellerLogin),
                buyerLogin_(buyerLogin)
{
}

std::string Transaction::toString()
{
    return (buyerLogin_ + " приобрёл у " + sellerLogin_ + " " +
            std::to_string(purchasedCurrency_) + " долларов, потравив " + std::to_string(currencySpent_) + " рублей\n");
}

std::string Transaction::toStringAsNotification(const std::string& userLogin)
{
    std::string str;
    if (userLogin == buyerLogin_)
    {
        str = "Вы приобрели " + std::to_string(purchasedCurrency_) + " долларов у " + sellerLogin_ +
            ", потратив " + std::to_string(currencySpent_) + " рублей\n";
    }
    else
    {
        str = buyerLogin_ + " приобрёл у вас " + std::to_string(purchasedCurrency_) + " долларов, потратив " +
            std::to_string(currencySpent_) + " рублей\n";
    }
    return str;
}