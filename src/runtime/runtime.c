#include "commands/commands.h"

int main(int argc, char** argv) {
    char* target_file = "/tmp/AppDir.sqfs";
    char* mount_point = "/tmp/appdir";

    mount_squashfs_payload_forked(target_file, mount_point, 0);

    execute_apprun(target_file, mount_point, argv);

    return 0;
}
