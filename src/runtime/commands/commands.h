#ifndef APPIMAGE_TYPE3_RUNTIME_COMMANDS_H
#define APPIMAGE_TYPE3_RUNTIME_COMMANDS_H


#include "appimage-header.h"

/* Mounts the payload contained in <file> with <offset> at <mount_point>
 *
 *  Forks the current process and run the fuse mount procedure in a separated
 *  process.
 *
 * @param file
 * @param mount_point
 * @param offset
 * @param clear_mount_point_on_unmount
 * @return
 */
int mount_squashfs_payload_forked(char* file, size_t offset, char* mount_point, bool remove_mount_point_on_exit);

/* Mounts the payload contained in <file> with <offset> at <mount_point>
 *
 * @param file
 * @param mount_point
 * @param offset
 * @param control_pipe (if not NULL) a byte is written when the loop is ready.
 * @return
 */
int mount_squashfuse_payload(char* file, const char* mount_point, size_t offset, int* control_pipe);

void execute_apprun(const char* appimage_path, const char* appdir_path, char** argv);

#endif //APPIMAGE_TYPE3_RUNTIME_COMMANDS_H
