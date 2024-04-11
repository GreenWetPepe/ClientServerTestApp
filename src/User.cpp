#include <iostream>
#include <string>
#include <vector>

#include "Common.hpp"

class User
{
public:
    User(std::string password) : password_(password)
    {
    }

    std::string getUserData()
    {
        return "Баланс:\n" + std::to_string(usdBalance_) + " Долларов\n" +
            std::to_string(rubBalance_) + " Рублей\n";
    }

    int getUsdBalance() const { return usdBalance_; }
    int getRubBalance() const { return rubBalance_; }

    void applyRubBalance(int deltRub)
    {
        rubBalance_ += deltRub;
    }

    void applyUsdBalance(int deltUsd)
    {
        usdBalance_ += deltUsd;
    }

    std::string getPassword() const { return password_; }

    std::string getNotifications()
    {
        std::string ans = notifications_;
        notifications_ = "";
        return ans;
    }

    void addNotification(const std::string& notification)
    {
        notifications_ += notification;
    }

private:
    std::string password_;
    std::string notifications_ = "";

    int rubBalance_ = 0;
    int usdBalance_ = 0;
};

class Deal
{
public:
    Deal(int requiredQuantity, int requiredPayment, bool dealType, std::string ownerLogin) : 
        requiredQuantity_(requiredQuantity),
        requiredPayment_(requiredPayment),
        dealType_(dealType),
        ownerLogin_(ownerLogin)
    {
    }

    std::string toString()
    {
        return ownerLogin_ + (dealType_ == DealType::BUY ? " покупает " : " продаёт ") +
            std::to_string(requiredQuantity_) + " долларов по цене " + std::to_string(requiredPayment_) +
            " рублей за ед.\n";
    }

    unsigned int getRequiredQuantity() const { return requiredQuantity_; }
    unsigned int getRequiredPayment() const { return requiredPayment_; }
    int getDealType() const { return dealType_; }
    std::string getOwnerLogin() const { return ownerLogin_; }

    void applyRequiredQuantity(unsigned int deltQuantity)
    {
        requiredQuantity_ += deltQuantity;
    }

private:
    unsigned int requiredQuantity_, requiredPayment_;
    int dealType_;
    std::string ownerLogin_;
};

class Transaction
{
public:
    Transaction(unsigned int purchasedCurrency, unsigned int currencySpent,
                std::string sellerLogin, std::string buyerLogin) :
                    purchasedCurrency_(purchasedCurrency),
                    currencySpent_(currencySpent),
                    sellerLogin_(sellerLogin),
                    buyerLogin_(buyerLogin)
    {
    }

    std::string toString()
    {
        return (buyerLogin_ + " приобрёл у " + sellerLogin_ + " " +
                std::to_string(purchasedCurrency_) + " долларов, потравив " + std::to_string(currencySpent_) + " рублей\n");
    }

    std::string toStringAsNotification(const std::string& userLogin)
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

private:
    unsigned int purchasedCurrency_, currencySpent_;
    std::string sellerLogin_,  buyerLogin_;
};