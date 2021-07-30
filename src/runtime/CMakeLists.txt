import_pkgconfig_target(TARGET_NAME fuse2 PKGCONFIG_TARGET fuse)
import_pkgconfig_target(TARGET_NAME zlib PKGCONFIG_TARGET zlib)
import_pkgconfig_target(TARGET_NAME liblzma PKGCONFIG_TARGET liblzma)

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
    PRIVATE bson_static appimage_type3_header
            libsquashfuse fuse2 zlib liblzma pthread dl
)

target_compile_definitions(runtime PRIVATE _FILE_OFFSET_BITS=64)

# add magic bytes section to the final executable
target_link_options(runtime PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/magic_bytes.ld)