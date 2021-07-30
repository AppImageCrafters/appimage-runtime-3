#ifndef APPIMAGE_TYPE3_RUNTIME_SETTINGS_H
#define APPIMAGE_TYPE3_RUNTIME_SETTINGS_H

#include <stdbool.h>

typedef struct {
    char* target;
    char* mount_point;
    bool remove_mount_point_on_exit;
    char* print_metadata;
} runtime_settings;

runtime_settings load_settings();

#endif //APPIMAGE_TYPE3_RUNTIME_SETTINGS_H
