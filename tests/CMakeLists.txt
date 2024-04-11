

ADD_EXECUTABLE(CoreTests Tests.cpp ../src/Core.cpp)
TARGET_LINK_LIBRARIES(CoreTests gtest gtest_main)