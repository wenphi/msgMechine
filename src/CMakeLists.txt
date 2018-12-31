include_directories(${CMAKE_HOME_DIRECTORY}/depend/zeroMQ/include)
include_directories(${CMAKE_HOME_DIRECTORY}/src)

aux_source_directory(${CMAKE_CURRENT_LIST_DIR}/robotStateMechine SRC_STATE)
add_library(libsrc SHARED ${SRC_STATE})