#include <string>


class Deal
{
public:
    Deal(int requiredQuantity, int requiredPayment, bool dealType, std::string ownerLogin);

    std::string toString();

    unsigned int getRequiredQuantity() const;
    unsigned int getRequiredPayment() const;
    int getDealType() const;
    std::string getOwnerLogin() const;

    void applyRequiredQuantity(unsigned int deltQuantity);

private:
    unsigned int requiredQuantity_, requiredPayment_;
    int dealType_;
    std::string ownerLogin_;
};