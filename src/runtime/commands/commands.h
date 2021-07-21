#ifndef APPIMAGE_TYPE3_RUNTIME_COMMANDS_H
#define APPIMAGE_TYPE3_RUNTIME_COMMANDS_H


#include "appimage-header.h"

/* Mounts the payload contained in <file> with <offset> at <mount_point>
 *
 * This starts a fuse worker in a separated process which must be stopped
 * on completion.
 *
 * Returns: service worker PID or a value lower than 0 in case of error. *
 * */
int mount_payload(char* file, char* mount_point, size_t offset);

#endif //APPIMAGE_TYPE3_RUNTIME_COMMANDS_H
