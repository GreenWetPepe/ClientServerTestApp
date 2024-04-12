#include <iostream>
#include <string>
#include <vector>

#include "User.h"
#include "Common.hpp"


User::User(std::string password) : password_(password)
{
}

std::string User::getUserData()
{
    return "Баланс:\n" + std::to_string(usdBalance_) + " Долларов\n" +
        std::to_string(rubBalance_) + " Рублей\n";
}

int User::getUsdBalance() const { return usdBalance_; }
int User::getRubBalance() const { return rubBalance_; }

void User::applyRubBalance(int deltRub)
{
    rubBalance_ += deltRub;
}

void User::applyUsdBalance(int deltUsd)
{
    usdBalance_ += deltUsd;
}

std::string User::getPassword() const { return password_; }

std::string User::getNotifications()
{
    std::string ans = notifications_;
    notifications_ = "";
    return ans;
}

void User::addNotification(const std::string& notification)
{
    notifications_ += notification;
}
