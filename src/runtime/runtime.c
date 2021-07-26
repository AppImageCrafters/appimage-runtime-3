#include <stdio.h>
#include "settings.h"
#include "commands/commands.h"


int main(int argc, char** argv) {
    runtime_settings settings = load_settings();

    appimage_header_t* header = read_appimage_header(settings.target);
    if (header == NULL) {
        fprintf(stderr, "\nERROR: Missing AppImage header\n");
        return -1;
    }

    int status = mount_squashfs_payload_forked(settings.target, header->payload_offset, settings.mount_point,
                                               settings.remove_mount_point_on_exit);
    if (status != 0) {
        fprintf(stderr, "\nINFO: Please check:\n"
                        "- the AppImage file is complete and is not corrupted\n"
                        "- fuse is properly installed and configured in your system\n"
        );

        return status;
    }

    return execute_apprun(settings.target, settings.mount_point, argv);
}
