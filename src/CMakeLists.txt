

ADD_EXECUTABLE(Server Server.cpp Core.cpp User.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Server PRIVATE Threads::Threads ${Boost_LIBRARIES})

ADD_EXECUTABLE(Client Client.cpp Common.hpp json.hpp)
TARGET_LINK_LIBRARIES(Client PRIVATE Threads::Threads ${Boost_LIBRARIES})