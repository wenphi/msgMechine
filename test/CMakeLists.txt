include_directories(${CMAKE_HOME_DIRECTORY}/src)

#zmq
include_directories(${CMAKE_HOME_DIRECTORY}/depend/zeroMQ/include)
link_directories(${CMAKE_HOME_DERECTORY}/depend/zeroMQ/libzmq)

add_executable(test1 test1.cpp)
target_link_libraries(test1 jsoncpp zmq)

add_executable(testApi testApi.cpp)
target_link_libraries(testApi jsoncpp zmq)