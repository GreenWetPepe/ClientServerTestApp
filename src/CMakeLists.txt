FIND_PACKAGE(Boost 1.40 COMPONENTS system REQUIRED)
FIND_PACKAGE(Threads REQUIRED)

ADD_EXECUTABLE(Server MainServer.cpp Server.cpp Session.cpp Core.cpp User.cpp Deal.cpp Transaction.cpp Common.hpp json.hpp)
TARGET_INCLUDE_DIRECTORIES(Server PRIVATE ${ClienServerEcn_SOURCE_DIR}/include ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads)

ADD_EXECUTABLE(Client MainClient.cpp Common.hpp json.hpp)
TARGET_INCLUDE_DIRECTORIES(Client PRIVATE ${ClienServerEcn_SOURCE_DIR}/include ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads)