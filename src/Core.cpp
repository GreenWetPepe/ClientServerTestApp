#include <cstdlib>
#include <iostream>
#include <sstream>
#include <memory>
#include <algorithm>
#include <map>

#include "User.cpp"
#include "Common.hpp"

class Core
{
public:
    // "Регистрирует" нового пользователя и возвращает его ID.
    std::string RegisterNewUser(const std::string& message)
    {
        std::vector<std::string> userData = ParseStringBySpaces(message);
        mUsers[userData[0]] = new User(userData[1]);

        return ServerResponses::LOG_IN_COMPLETE;
    }

    std::string LogInUser(const std::string& userLogin, const std::string& password)
    {
        if (mUsers[userLogin]->getPassword() == password) return ServerResponses::LOG_IN_COMPLETE;
        return ServerResponses::INVALID_PASSWORD;
    }

    std::string CheckLoginAvailability(const std::string& aUserLogin)
    {
        if (IsLoginExists(aUserLogin)) return ServerResponses::LOGIN_TAKEN;
        return ServerResponses::LOGIN_FREE;
    }

    std::string GetUserData(const std::string& aUserLogin)
    {
        if (!IsLoginExists(aUserLogin)) return "Error! Unknown User";
        return "Логин: " + aUserLogin + "\n" + mUsers[aUserLogin]->getUserData();
    }

    std::string AddDeal(const std::string& userLogin, const std::string& message)
    {
        std::vector<std::string> dataCollection = ParseStringBySpaces(message);
        
        if (dataCollection.size() != 3) return "Ошибка в добавлении сделки";

        try
        {
            int dealType = std::stoi(dataCollection[0]);
            int requiredQuantity = std::stoi(dataCollection[1]);
            int requiredPayment = std::stoi(dataCollection[2]);
            deals.push_back(std::make_shared<Deal>(requiredQuantity, requiredPayment, dealType, userLogin));
            // std::cout << "Сделка создана " << deals.back()->getRequiredQuantity() << " " <<
            //             deals.back()->getRequiredPayment() << " " << deals.back()->getOwnerLogin() << "\n";
            return "Сделка успешно добавлена";
        }
        catch(const std::exception& e)
        {
            std::cerr << "Exception " << e.what() << '\n';
        }

        return "Ошибка в добавлении сделки";
    }

    void ProcessLastDeal()
    {
        if (deals.empty()) return;
        Deal *d1 = deals.back().get();
        std::vector<std::shared_ptr<Deal>> suitableDeals = GetSuitableDeals(d1);

        while (d1->getRequiredQuantity() != 0 && !suitableDeals.empty())
        {
            unsigned int maxPayment = suitableDeals[0]->getRequiredPayment();
            size_t maxPaymentId = 0;

            for (size_t i = 0; i < suitableDeals.size(); i++)
            {
                if (suitableDeals[i]->getRequiredPayment() > maxPayment)
                {
                    maxPayment = suitableDeals[i]->getRequiredPayment();
                    maxPaymentId = i;
                }
            }

            CompleteDeal(d1, suitableDeals[maxPaymentId].get());

            if (suitableDeals[maxPaymentId]->getRequiredQuantity() == 0)
            {
                DeleteTrade(suitableDeals[maxPaymentId].get());
                suitableDeals.erase(suitableDeals.begin() + maxPaymentId);
            }
        }
        if (d1->getRequiredQuantity() == 0) DeleteTrade(d1);
    }

    std::string getDealsString(std::string filter)
    {
        std::string ans = "";
        for (auto deal : deals)
        {
            if (!filter.empty() && deal->getOwnerLogin() != filter) continue;
            ans += deal->toString();
        }
        return ans;
    }

    std::string getTransactionsString()
    {
        std::string ans = "";
        for (auto transaction : transactions)
        {
            ans += transaction.toString();
        }
        return ans;
    }

    std::string getNotifications(const std::string& userLogin)
    {
        return mUsers[userLogin]->getNotifications();
    }

    const Deal* getDeal(int index) 
    {
        if (deals.size() <= index) return nullptr;
        return deals[index].get();
    }

    int getDealsCount() { return deals.size(); }

    const User* getUser(const std::string& aUserLogin)
    {
        if (!IsLoginExists(aUserLogin)) return nullptr;
        return mUsers[aUserLogin]; 
    }

private:
    // <UserLogin, User>
    std::map<std::string, User*> mUsers;
    std::vector<std::shared_ptr<Deal>> deals;
    std::vector<Transaction> transactions;

    bool IsLoginExists(const std::string& aUserLogin)
    {
        const auto userIt = mUsers.find(aUserLogin);
        if (userIt == mUsers.cend())
        {
            return false;
        }
        return true;
    }

    void CompleteDeal(Deal *d1, Deal *d2)
    {
        unsigned int dealAmount = std::min(d1->getRequiredQuantity(), d2->getRequiredQuantity());
        unsigned int dealPayment;
        if (d1->getDealType() == DealType::BUY) dealPayment = d1->getRequiredPayment();
        else dealPayment = d2->getRequiredPayment();
        d1->applyRequiredQuantity(-dealAmount);
        d2->applyRequiredQuantity(-dealAmount);

        std::string buyerLogin, sellerLogin;
        if (d1->getDealType() == DealType::BUY)
        {
            mUsers[d1->getOwnerLogin()]->applyUsdBalance(dealAmount);
            mUsers[d1->getOwnerLogin()]->applyRubBalance(-dealAmount * dealPayment);
            mUsers[d2->getOwnerLogin()]->applyUsdBalance(-dealAmount);
            mUsers[d2->getOwnerLogin()]->applyRubBalance(dealAmount * dealPayment);
            buyerLogin = d1->getOwnerLogin();
            sellerLogin = d2->getOwnerLogin();
        }
        else
        {
            mUsers[d2->getOwnerLogin()]->applyUsdBalance(dealAmount);
            mUsers[d2->getOwnerLogin()]->applyRubBalance(-dealAmount * dealPayment);
            mUsers[d1->getOwnerLogin()]->applyUsdBalance(-dealAmount);
            mUsers[d1->getOwnerLogin()]->applyRubBalance(dealAmount * dealPayment);
            buyerLogin = d2->getOwnerLogin();
            sellerLogin = d1->getOwnerLogin();
        }
        transactions.push_back(Transaction(dealAmount, dealAmount * dealPayment, sellerLogin, buyerLogin));
        mUsers[d1->getOwnerLogin()]->addNotification(transactions.back().toStringAsNotification(d1->getOwnerLogin()));
        mUsers[d2->getOwnerLogin()]->addNotification(transactions.back().toStringAsNotification(d2->getOwnerLogin()));
    }

    std::vector<std::shared_ptr<Deal>> GetSuitableDeals(Deal* deal)
    {
        int requiredDealType = deal->getDealType() == DealType::BUY ? DealType::SELL : DealType::BUY;
        std::vector<std::shared_ptr<Deal>> suitableDeals;
        for (size_t i = 0; i < deals.size(); i++)
        {
            if (deal->getOwnerLogin() != deals[i]->getOwnerLogin() && deals[i]->getDealType() == requiredDealType)
            {
                if (requiredDealType == DealType::BUY && deals[i]->getRequiredPayment() >= deal->getRequiredPayment())
                {
                    suitableDeals.push_back(deals[i]);
                }
                else if (requiredDealType == DealType::SELL && deals[i]->getRequiredPayment() <= deal->getRequiredPayment())
                {
                    suitableDeals.push_back(deals[i]);
                }
            }
        }
        return suitableDeals;
    }

    void DeleteTrade(Deal* deal)
    {
        for (size_t i = 0; i < deals.size(); i++)
        {
            if (deal == deals[i].get())
            {
                deals.erase(deals.begin() + i);
                return;
            }
        }
    }

    std::vector<std::string> ParseStringBySpaces(const std::string& str)
    {
        std::istringstream iss(str);
        std::vector<std::string> dataCollection;
        std::string data;
        while(iss >> data)
        {
            if (!data.empty()) dataCollection.push_back(data);
        }
        return dataCollection;
    }

};