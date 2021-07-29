# this one is important
SET(CMAKE_SYSTEM_NAME Linux)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(HOST i386-linux-gnu)

# specify the cross compiler
SET(CMAKE_C_COMPILER   /usr/bin/gcc)
SET(CMAKE_C_COMPILER_ARG1 "-m32")
SET(CMAKE_CXX_COMPILER /usr/bin/g++)
SET(CMAKE_CXX_COMPILER_ARG1 "-m32")

# where is the target environment
SET(CMAKE_FIND_ROOT_PATH /usr/lib/${HOST})
SET(CMAKE_IGNORE_PATH /usr/lib/x86_64-linux-gnu/ /usr/lib/x86_64-linux-gnu/lib/)

# Set PKG CONFIG files lookup path
set(ENV{PKG_CONFIG_PATH} "/usr/lib/${HOST}/pkgconfig/")

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
