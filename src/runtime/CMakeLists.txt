import_pkgconfig_target(TARGET_NAME fuse PKGCONFIG_TARGET fuse3)
import_pkgconfig_target(TARGET_NAME zlib PKGCONFIG_TARGET zlib)
import_pkgconfig_target(TARGET_NAME liblzma PKGCONFIG_TARGET liblzma)

add_executable(runtime runtime.c commands/mount.c)
target_link_libraries(runtime PRIVATE libsquashfuse fuse3 zlib liblzma  appimage_type3_header pthread dl)
target_compile_definitions(runtime PRIVATE FUSE_USE_VERSION=32)

# add magic bytes section to the final executable
target_link_options(runtime PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/magic_bytes.ld)