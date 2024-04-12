#include <string>

class Transaction
{
public:
    Transaction(unsigned int purchasedCurrency, unsigned int currencySpent,
                std::string sellerLogin, std::string buyerLogin);

    std::string toString();
    std::string toStringAsNotification(const std::string& userLogin);

private:
    unsigned int purchasedCurrency_, currencySpent_;
    std::string sellerLogin_,  buyerLogin_;
};