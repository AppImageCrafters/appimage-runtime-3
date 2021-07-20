# >= 3.2 required for ExternalProject_Add_StepDependencies
cmake_minimum_required(VERSION 3.2)

include(${CMAKE_CURRENT_LIST_DIR}/scripts.cmake)


# as distros don't provide suitable squashfuse and squashfs-tools, those dependencies are bundled in, can, and should
# be used from this repository for AppImageKit
# for distro packaging, it can be linked to an existing package just fine
set(USE_SYSTEM_SQUASHFUSE OFF CACHE BOOL "Use system libsquashfuse instead of building our own")

if(NOT USE_SYSTEM_SQUASHFUSE)
    message(STATUS "Downloading and building squashfuse")

    # Check if fuse is installed to provide early error reports
    import_pkgconfig_target(TARGET_NAME libfuse PKGCONFIG_TARGET fuse)

    ExternalProject_Add(
        squashfuse-EXTERNAL
        GIT_REPOSITORY https://github.com/vasi/squashfuse/
        GIT_TAG master
        UPDATE_COMMAND ""  # make sure CMake won't try to fetch updates unnecessarily and hence rebuild the dependency every time
        CONFIGURE_COMMAND sh autogen.sh
        COMMAND sh <SOURCE_DIR>/configure --disable-demo --disable-high-level --without-lzo --without-lz4 --prefix=<INSTALL_DIR> --libdir=<INSTALL_DIR>/lib --with-xz=${xz_PREFIX}
        BUILD_COMMAND ${MAKE}
        BUILD_IN_SOURCE ON
        INSTALL_COMMAND ${MAKE} install
        UPDATE_DISCONNECTED On
    )

    import_external_project(
        TARGET_NAME libsquashfuse
        EXT_PROJECT_NAME squashfuse-EXTERNAL
        LIBRARIES "<SOURCE_DIR>/.libs/libfuseprivate.a;<SOURCE_DIR>/.libs/libsquashfuse.a;<SOURCE_DIR>/.libs/libsquashfuse_convenience.a;<SOURCE_DIR>/.libs/libsquashfuse_ll.a;<SOURCE_DIR>/.libs/libsquashfuse_ll_convenience.a"
        INCLUDE_DIRS "<SOURCE_DIR>"
    )
else()
    message(STATUS "Using system squashfuse")

    import_pkgconfig_target(TARGET_NAME libsquashfuse PKGCONFIG_TARGET squashfuse)
endif()
