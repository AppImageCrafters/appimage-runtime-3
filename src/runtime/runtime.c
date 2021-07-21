#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

#include "commands/commands.h"

int main(int argc, char** argv) {
    char* target_file = "/home/alexis/Workspace/third_party/curl-appimage/AppDir.sqfs";
    char* mount_point = "/tmp/appdir";

    /* Code executed by parent */
    int err = mount_payload(target_file, mount_point, 0);
    if (err) {
        fprintf(stderr, "Payload mount failed with code: %d\n", err);
        return err;
    }
    return 0;
}
