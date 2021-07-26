#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


void set_runtime_environment(const char* appimage_path, const char* appdir_path) {
    setenv("APPIMAGE", appimage_path, 1);
    setenv("APPDIR", appdir_path, 1);
}

char* create_apprun_path(const char* appdir_path) {
    size_t appdir_path_len = strlen(appdir_path);
    size_t apprun_path_len = appdir_path_len + 8;

    char* apprun_path = malloc(apprun_path_len);
    memset(apprun_path, 0, apprun_path_len);

    strcat(apprun_path, appdir_path);
    strcat(apprun_path, "/AppRun");

    return apprun_path;
}

void execute_apprun(const char* appimage_path, const char* appdir_path, char** argv) {
    char* apprun_path = create_apprun_path(appdir_path);

    set_runtime_environment(appimage_path, appdir_path);

    argv[0] = apprun_path;
    execv(apprun_path, argv);
    fprintf(stderr, "Unable to run: %s", apprun_path);
}
