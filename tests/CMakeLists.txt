ENABLE_TESTING()

ADD_EXECUTABLE(CoreTests Tests.cpp ../src/Core.cpp ../src/Deal.cpp ../src/User.cpp ../src/Transaction.cpp)
TARGET_INCLUDE_DIRECTORIES(CoreTests PRIVATE ${ClienServerEcn_SOURCE_DIR}/include) 
TARGET_LINK_LIBRARIES(CoreTests PRIVATE gtest gtest_main)

ADD_TEST(CoreTests CoreTests)