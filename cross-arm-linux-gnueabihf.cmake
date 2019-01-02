# arm编译
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_C_COMPILER   /usr/bin/arm-linux-gnueabihf-gcc-4.8)
set(CMAKE_CXX_COMPILER /usr/bin/arm-linux-gnueabihf-g++-4.8)


# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /usr/arm-linux-gnueabihf)

# search for programs in the build host directories (not necessary)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE armhf)


set(BUILD_OS_ARM_LINUX true)
add_definitions(-DBUILD_OS_ARM_LINUX)