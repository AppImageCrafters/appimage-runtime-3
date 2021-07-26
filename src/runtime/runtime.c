#include "settings.h"
#include "commands/commands.h"


int main(int argc, char** argv) {
    runtime_settings settings = load_settings();

    mount_squashfs_payload_forked(settings.target, 0, settings.mount_point, settings.remove_mount_point_on_exit);

    execute_apprun(settings.target, settings.mount_point, argv);

    return 0;
}
