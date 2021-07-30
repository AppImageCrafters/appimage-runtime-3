#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#include "settings.h"


char* resolve_exe_path() {
    char buf[FILENAME_MAX] = {0x0};

    ssize_t ret = readlink("/proc/self/exe", buf, FILENAME_MAX);
    if (ret == -1) {
        perror("readlink");
        exit(1);
    }

    return strndup(buf, (unsigned long) ret);
}

char* create_default_mount_point() {
    char path_template[] = "/tmp/appdir.XXXXXX";
    return strdup(mkdtemp(path_template));
}

runtime_settings load_settings() {
    runtime_settings settings = {0x0};

    settings.target = getenv("APPIMAGE_OPT_FILE");
    settings.mount_point = getenv("APPIMAGE_OPT_MOUNT_POINT");

    settings.print_metadata = getenv("APPIMAGE_OPT_PRINT_METADATA");
    if (settings.print_metadata)
        unsetenv("APPIMAGE_OPT_PRINT_METADATA");

    if (settings.target == NULL)
        settings.target = resolve_exe_path();
    else
        unsetenv("APPIMAGE_OPT_FILE");

    if (settings.mount_point == NULL) {
        settings.mount_point = create_default_mount_point();
        settings.remove_mount_point_on_exit = true;
    } else
        unsetenv("APPIMAGE_OPT_MOUNT_POINT");

    return settings;
}
