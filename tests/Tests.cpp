#include <gtest/gtest.h>

#include "../src/Core.cpp"

TEST(TestCoreDealsProcess, DealProcessingWithoutTransaction)
{
    Core core;
    core.RegisterNewUser("1 1");
    core.RegisterNewUser("2 2");

    core.AddDeal("1", "0 30 45");
    core.AddDeal("2", "1 20 50");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDeal(0)->getRequiredQuantity() == 30);
    ASSERT_TRUE(core.getDeal(1)->getRequiredQuantity() == 20);
    ASSERT_TRUE(core.getUser("1")->getUsdBalance() == 0);
    ASSERT_TRUE(core.getUser("2")->getUsdBalance() == 0);
}

TEST(TestCoreDealsProcess, DealProcessingWithSameType)
{
    Core core;
    core.RegisterNewUser("1 1");
    core.RegisterNewUser("2 2");

    core.AddDeal("1", "1 30 50");
    core.AddDeal("2", "1 20 50");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDeal(0)->getRequiredQuantity() == 30);
    ASSERT_TRUE(core.getDeal(1)->getRequiredQuantity() == 20);
    ASSERT_TRUE(core.getUser("1")->getUsdBalance() == 0);
    ASSERT_TRUE(core.getUser("2")->getUsdBalance() == 0);
}

TEST(TestCoreDealsProcess, ComparableDealsFromSameUser)
{
    Core core;
    core.RegisterNewUser("1 1");

    core.AddDeal("1", "0 30 45");
    core.AddDeal("1", "1 20 30");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDeal(0)->getRequiredQuantity() == 30);
}

TEST(TestCoreDealsProcess, DealFullExecution)
{
    Core core;
    core.RegisterNewUser("1 1");
    core.RegisterNewUser("2 2");

    core.AddDeal("1", "0 20 45");
    core.AddDeal("2", "1 20 30");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDealsCount() == 0);
    ASSERT_TRUE(core.getUser("1")->getRubBalance() == -20 * 45);
    ASSERT_TRUE(core.getUser("1")->getUsdBalance() == 20);
    ASSERT_TRUE(core.getUser("2")->getRubBalance() == 20 * 45);
    ASSERT_TRUE(core.getUser("2")->getUsdBalance() == -20);
}

TEST(TestCoreDealsProcess, DealPartialExecution)
{
    Core core;
    core.RegisterNewUser("1 1");
    core.RegisterNewUser("2 2");

    core.AddDeal("1", "0 10 45");
    core.AddDeal("2", "1 20 30");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDealsCount() == 1);
    ASSERT_TRUE(core.getDeal(0)->getRequiredQuantity() == 10);
    ASSERT_TRUE(core.getUser("1")->getUsdBalance() == 10);
    ASSERT_TRUE(core.getUser("2")->getUsdBalance() == -10);
}

TEST(TestCoreDealsProcess, PartialDealWithMultipleUsers)
{
    Core core;
    core.RegisterNewUser("1 1");
    core.RegisterNewUser("2 2");
    core.RegisterNewUser("3 3");

    core.AddDeal("1", "0 10 62");
    core.AddDeal("2", "0 20 63");
    core.AddDeal("3", "1 25 61");

    core.ProcessLastDeal();
    ASSERT_TRUE(core.getDealsCount() == 1);
    ASSERT_TRUE(core.getDeal(0)->getRequiredQuantity() == 5);
    ASSERT_TRUE(core.getUser("1")->getUsdBalance() == 5);
    ASSERT_TRUE(core.getUser("2")->getUsdBalance() == 20);
    ASSERT_TRUE(core.getUser("3")->getUsdBalance() == -25);
}


int main(int argc, char **argv)
{
    ::testing::InitGoogleTest();

    return RUN_ALL_TESTS();
}