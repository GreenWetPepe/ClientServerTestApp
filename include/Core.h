#include <iostream>
#include <memory>
#include <map>
#include <vector>

#include "User.h"
#include "Deal.h"
#include "Transaction.h"

class Core
{
public:
    std::string RegisterNewUser(const std::string& message);
    std::string LogInUser(const std::string& userLogin, const std::string& password);
    std::string CheckLoginAvailability(const std::string& aUserLogin);

    const User* GetUser(const std::string& aUserLogin);
    const Deal* GetDeal(int index);
    std::string GetUserData(const std::string& aUserLogin);
    std::string GetDealsAsString(const std::string& filter);
    std::string GetTransactionsString();
    std::string GetNotifications(const std::string& userLogin);
    int GetDealsCount();

    std::string AddDeal(const std::string& userLogin, const std::string& message);
    void ProcessLastDeal();

private:
    // <UserLogin, User>
    std::map<std::string, std::unique_ptr<User>> users_;
    std::vector<std::shared_ptr<Deal>> deals_;
    std::vector<Transaction> transactions_;

    bool IsLoginExists(const std::string& aUserLogin);
    void CompleteDeal(Deal *d1, Deal *d2);
    std::vector<std::shared_ptr<Deal>> GetSuitableDeals(Deal* deal);
    void DeleteTrade(Deal* deal);
    std::vector<std::string> ParseStringBySpaces(const std::string& str);

};