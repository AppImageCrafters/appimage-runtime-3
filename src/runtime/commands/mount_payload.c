#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <libgen.h>

// squashfuse headers
#include "ll.h"
#include "nonstd.h"

// app headers
#include "commands.h"

/**
 * Prepare low level filesystem operations struct to be used by fuse
 */
struct fuse_lowlevel_ops init_fuse_operations() {
    struct fuse_lowlevel_ops sqfs_ll_ops = {0x0};

    sqfs_ll_ops.getattr = sqfs_ll_op_getattr;
    sqfs_ll_ops.opendir = sqfs_ll_op_opendir;
    sqfs_ll_ops.releasedir = sqfs_ll_op_releasedir;
    sqfs_ll_ops.readdir = sqfs_ll_op_readdir;
    sqfs_ll_ops.lookup = sqfs_ll_op_lookup;
    sqfs_ll_ops.open = sqfs_ll_op_open;
    sqfs_ll_ops.create = sqfs_ll_op_create;
    sqfs_ll_ops.release = sqfs_ll_op_release;
    sqfs_ll_ops.read = sqfs_ll_op_read;
    sqfs_ll_ops.readlink = sqfs_ll_op_readlink;
    sqfs_ll_ops.listxattr = sqfs_ll_op_listxattr;
    sqfs_ll_ops.getxattr = sqfs_ll_op_getxattr;
    sqfs_ll_ops.forget = sqfs_ll_op_forget;
    sqfs_ll_ops.statfs = stfs_ll_op_statfs;

    return sqfs_ll_ops;
}

/**
 * Configures fuse params to be used.
 *
 * See man page "mount.fuse" for more details about the allowed options.
 * */
struct fuse_args init_fuse_args(char* image) {
    struct fuse_args fuse_args = {0x0};
    fuse_args.allocated = 1;

    fuse_args.argc = 4;
    fuse_args.argv = malloc(sizeof(char*) * (unsigned long) fuse_args.argc);
    fuse_args.argv[0] = strdup(image);

    // read only
    fuse_args.argv[1] = strdup("-oro");

    // If the kernel suppports it,  /etc/mtab  and /proc/mounts will show the filesystem type as fuse.TYPE
    char subtype_option[FILENAME_MAX] = {0x0};
    sprintf(subtype_option, "-osubtype=%s", basename(image));
    fuse_args.argv[2] = strdup(subtype_option);

    // auto umount (useful in case of failure)
    fuse_args.argv[3] = strdup("-oauto_unmount");

    return fuse_args;
}


int
start_fuse(const char* mount_point, struct fuse_lowlevel_ops* sqfs_ll_ops, sqfs_ll* ll, struct fuse_args* fuse_args) {
    sqfs_ll_chan ch;
    sqfs_err mount_res = sqfs_ll_mount(&ch, mount_point, fuse_args, sqfs_ll_ops, sizeof((*sqfs_ll_ops)), ll);


    if (mount_res == SQFS_OK) {
        // assume failure if fuse_session_loop never gets called

        int fuse_session_res = -1;
        if (fuse_set_signal_handlers(ch.session) != -1) {
            fuse_session_res = fuse_session_loop(ch.session);
            fuse_remove_signal_handlers(ch.session);
        }

        sqfs_ll_destroy(ll);
        sqfs_ll_unmount(&ch, mount_point);

        return fuse_session_res;
    } else {
        return (int) mount_res;
    }
}

int mount_payload(char* file, char* mount_point, size_t offset) {
    struct fuse_lowlevel_ops sqfs_ll_ops = init_fuse_operations();
    struct fuse_args fuse_args = init_fuse_args(file);

    int err;
    sqfs_ll* ll;

    /* OPEN FS */
    err = !(ll = sqfs_ll_open(file, offset));

    if (!err)
        err = start_fuse(mount_point, &sqfs_ll_ops, ll, &fuse_args);

    free(ll);
    fuse_opt_free_args(&fuse_args);
    return -err;
}
