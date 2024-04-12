#include <sstream>
#include <algorithm>

#include "Core.h"
#include "Common.hpp"


std::string Core::RegisterNewUser(const std::string& message)
{
    std::vector<std::string> userData = ParseStringBySpaces(message);
    users_[userData[0]] = std::make_unique<User>(userData[1]);

    return ServerResponses::LOG_IN_COMPLETE;
}

std::string Core::LogInUser(const std::string& userLogin, const std::string& password)
{
    if (users_[userLogin]->getPassword() == password) return ServerResponses::LOG_IN_COMPLETE;
    return ServerResponses::INVALID_PASSWORD;
}

std::string Core::CheckLoginAvailability(const std::string& aUserLogin)
{
    if (IsLoginExists(aUserLogin)) return ServerResponses::LOGIN_TAKEN;
    return ServerResponses::LOGIN_FREE;
}

std::string Core::GetUserData(const std::string& aUserLogin)
{
    if (!IsLoginExists(aUserLogin)) return "Error! Unknown User";
    return "Логин: " + aUserLogin + "\n" + users_[aUserLogin]->getUserData();
}

std::string Core::AddDeal(const std::string& userLogin, const std::string& message)
{
    std::vector<std::string> dataCollection = ParseStringBySpaces(message);
    
    if (dataCollection.size() != 3) return "Ошибка в добавлении сделки";

    try
    {
        int dealType = std::stoi(dataCollection[0]);
        int requiredQuantity = std::stoi(dataCollection[1]);
        int requiredPayment = std::stoi(dataCollection[2]);
        deals_.push_back(std::make_shared<Deal>(requiredQuantity, requiredPayment, dealType, userLogin));
        return "Сделка успешно добавлена";
    }
    catch(const std::exception& e)
    {
        std::cerr << "Exception " << e.what() << '\n';
    }

    return "Ошибка в добавлении сделки";
}

// Обрабатывает последнее предложение
void Core::ProcessLastDeal()
{
    if (deals_.empty()) return;
    Deal *d1 = deals_.back().get();
    std::vector<std::shared_ptr<Deal>> suitableDeals = GetSuitableDeals(d1);

    while (d1->getRequiredQuantity() != 0 && !suitableDeals.empty())
    {
        unsigned int profitablePayment = suitableDeals[0]->getRequiredPayment();
        size_t profitablePaymentId = 0;

        // Ищем наиболее выгодное предложение
        for (size_t i = 0; i < suitableDeals.size(); i++)
        {
            if (suitableDeals[i]->getDealType() == DealType::BUY && suitableDeals[i]->getRequiredPayment() > profitablePayment ||
                suitableDeals[i]->getDealType() == DealType::SELL && suitableDeals[i]->getRequiredPayment() < profitablePayment)
            {
                profitablePaymentId = suitableDeals[i]->getRequiredPayment();
                profitablePaymentId = i;
            }
        }
        // Совершаем сделку
        CompleteDeal(d1, suitableDeals[profitablePaymentId].get());

        if (suitableDeals[profitablePaymentId]->getRequiredQuantity() == 0)
        {
            DeleteTrade(suitableDeals[profitablePaymentId].get());
            suitableDeals.erase(suitableDeals.begin() + profitablePaymentId);
        }
    }
    if (d1->getRequiredQuantity() == 0) DeleteTrade(d1);
}

std::string Core::GetDealsAsString(const std::string& filter)
{
    std::string ans = "";
    for (auto deal : deals_)
    {
        if (!filter.empty() && deal->getOwnerLogin() != filter) continue;
        ans += deal->toString();
    }
    return ans;
}

std::string Core::GetTransactionsString()
{
    std::string ans = "";
    for (auto transaction : transactions_)
    {
        ans += transaction.toString();
    }
    return ans;
}

std::string Core::GetNotifications(const std::string& userLogin)
{
    return users_[userLogin]->getNotifications();
}

const Deal* Core::GetDeal(int index) 
{
    if (deals_.size() <= index) return nullptr;
    return deals_[index].get();
}

int Core::GetDealsCount() { return deals_.size(); }

const User* Core::GetUser(const std::string& aUserLogin)
{
    if (!IsLoginExists(aUserLogin)) return nullptr;
    return users_[aUserLogin].get(); 
}

bool Core::IsLoginExists(const std::string& aUserLogin)
{
    const auto userIt = users_.find(aUserLogin);
    if (userIt == users_.cend())
    {
        return false;
    }
    return true;
}

// Совершает транзакцию между предложениями
void Core::CompleteDeal(Deal *d1, Deal *d2)
{
    unsigned int dealAmount = std::min(d1->getRequiredQuantity(), d2->getRequiredQuantity());
    unsigned int dealPayment = d2->getRequiredPayment(); // Берём требуемую цену у второго пользователя, т.к. она выгоднее нашего предложения

    // Уменьшаем требуемое кол-во валюты в сделках на объём нашей сделки
    d1->applyRequiredQuantity(-dealAmount);
    d2->applyRequiredQuantity(-dealAmount);

    // Изменяем баланс пользователей
    std::string buyerLogin, sellerLogin;
    if (d1->getDealType() == DealType::BUY)
    {
        users_[d1->getOwnerLogin()]->applyUsdBalance(dealAmount);
        users_[d1->getOwnerLogin()]->applyRubBalance(-dealAmount * dealPayment);
        users_[d2->getOwnerLogin()]->applyUsdBalance(-dealAmount);
        users_[d2->getOwnerLogin()]->applyRubBalance(dealAmount * dealPayment);
        buyerLogin = d1->getOwnerLogin();
        sellerLogin = d2->getOwnerLogin();
    }
    else
    {
        users_[d2->getOwnerLogin()]->applyUsdBalance(dealAmount);
        users_[d2->getOwnerLogin()]->applyRubBalance(-dealAmount * dealPayment);
        users_[d1->getOwnerLogin()]->applyUsdBalance(-dealAmount);
        users_[d1->getOwnerLogin()]->applyRubBalance(dealAmount * dealPayment);
        buyerLogin = d2->getOwnerLogin();
        sellerLogin = d1->getOwnerLogin();
    }
    // Создаём транзакцию и посылаем пользователям уведомление о совершённой сделке
    transactions_.push_back(Transaction(dealAmount, dealAmount * dealPayment, sellerLogin, buyerLogin));
    users_[d1->getOwnerLogin()]->addNotification(transactions_.back().toStringAsNotification(d1->getOwnerLogin()));
    users_[d2->getOwnerLogin()]->addNotification(transactions_.back().toStringAsNotification(d2->getOwnerLogin()));
}

// Возвращает предложения, совместимые с указанным
std::vector<std::shared_ptr<Deal>> Core::GetSuitableDeals(Deal* deal)
{
    int requiredDealType = deal->getDealType() == DealType::BUY ? DealType::SELL : DealType::BUY;
    std::vector<std::shared_ptr<Deal>> suitableDeals;
    for (size_t i = 0; i < deals_.size(); i++)
    {
        if (deal->getOwnerLogin() != deals_[i]->getOwnerLogin() && deals_[i]->getDealType() == requiredDealType)
        {
            if (requiredDealType == DealType::BUY && deals_[i]->getRequiredPayment() >= deal->getRequiredPayment())
            {
                suitableDeals.push_back(deals_[i]);
            }
            else if (requiredDealType == DealType::SELL && deals_[i]->getRequiredPayment() <= deal->getRequiredPayment())
            {
                suitableDeals.push_back(deals_[i]);
            }
        }
    }
    return suitableDeals;
}

void Core::DeleteTrade(Deal* deal)
{
    for (size_t i = 0; i < deals_.size(); i++)
    {
        if (deal == deals_[i].get())
        {
            deals_.erase(deals_.begin() + i);
            return;
        }
    }
}

// Делит строку на части и возвращает вектор слов(знаений)
std::vector<std::string> Core::ParseStringBySpaces(const std::string& str)
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