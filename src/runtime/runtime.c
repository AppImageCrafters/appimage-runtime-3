#include <stdio.h>
#include <sys/wait.h>

#include "commands/commands.h"

int main(int argc, char** argv) {
    int err = mount_payload("/home/alexis/Workspace/third_party/curl-appimage/AppDir.sqfs", "/tmp/AppDir", 0);
    if (err) {
        fprintf(stderr, "Payload mount failed with code: %d\n", err);
        return err;
    }
}
