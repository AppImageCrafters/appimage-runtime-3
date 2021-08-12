find_package(Threads REQUIRED)

include(FindPkgConfig)
pkg_check_modules(ZSTD REQUIRED IMPORTED_TARGET libzstd)
pkg_check_modules(FUSE REQUIRED IMPORTED_TARGET fuse)


add_executable(
    runtime

    runtime.c
    settings.c
    commands/mount_suqashfs_payload.c
    commands/launch_apprun.c
    commands/show_metadata.c
)

target_link_libraries(
    runtime
    PRIVATE appimage_type3_header libsquashfuse bson_static Threads::Threads PkgConfig::ZSTD PkgConfig::FUSE
)

target_compile_definitions(runtime PRIVATE _FILE_OFFSET_BITS=64)

# add magic bytes section to the final executable
target_link_options(runtime PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/magic_bytes.ld)
