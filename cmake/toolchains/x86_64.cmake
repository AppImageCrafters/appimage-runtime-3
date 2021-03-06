# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(HOST x86_64-linux-gnu)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /usr/bin/${HOST}-gcc)
SET(CMAKE_CXX_COMPILER /usr/bin/${HOST}-g++)

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /usr/lib/${HOST} /usr/lib/${HOST}/cmake/)

# Set PKG CONFIG files lookup path
set(ENV{PKG_CONFIG_PATH} "/usr/lib/${HOST}/pkgconfig/")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
