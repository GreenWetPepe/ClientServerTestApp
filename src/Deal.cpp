#include "Deal.h"
#include "Common.hpp"

Deal::Deal(int requiredQuantity, int requiredPayment, bool dealType, std::string ownerLogin) : 
    requiredQuantity_(requiredQuantity),
    requiredPayment_(requiredPayment),
    dealType_(dealType),
    ownerLogin_(ownerLogin)
{
}

std::string Deal::toString()
{
    return ownerLogin_ + (dealType_ == DealType::BUY ? " покупает " : " продаёт ") +
        std::to_string(requiredQuantity_) + " долларов по цене " + std::to_string(requiredPayment_) +
        " рублей за ед.\n";
}

unsigned int Deal::getRequiredQuantity() const { return requiredQuantity_; }
unsigned int Deal::getRequiredPayment() const { return requiredPayment_; }
int Deal::getDealType() const { return dealType_; }
std::string Deal::getOwnerLogin() const { return ownerLogin_; }

void Deal::applyRequiredQuantity(unsigned int deltQuantity)
{
    requiredQuantity_ += deltQuantity;
}
