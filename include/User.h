#include <string>


class User
{
public:
    User(std::string password);

    std::string getUserData();

    int getUsdBalance() const;
    int getRubBalance() const;

    std::string getPassword() const;
    std::string getNotifications();

    void applyRubBalance(int deltRub);
    void applyUsdBalance(int deltUsd);

    void addNotification(const std::string& notification);

private:
    std::string password_;
    std::string notifications_ = "";

    int rubBalance_ = 0;
    int usdBalance_ = 0;
};