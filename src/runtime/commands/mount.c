#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

// squashfuse headers
#include "ll.h"
#include "fuseprivate.h"
#include "nonstd.h"

// app headers
#include "commands.h"

/* Convenience struct grouping all the squashfuse runtime settings */
typedef struct {
    struct fuse_args args;
    sqfs_opts opts;
    struct fuse_cmdline_opts fuse_cmdline_opts;
    struct fuse_lowlevel_ops sqfs_ll_ops;
} squashfuse_setup;

squashfuse_setup* init_squashfuse(int argc, char** argv) {
    squashfuse_setup* setup = malloc(sizeof(squashfuse_setup));

#if FUSE_USE_VERSION >= 30
#else
    struct {
            char *mountpoint;
            int mt, foreground;
        } fuse_cmdline_opts;
#endif

    struct fuse_opt fuse_opts[] = {
            {"offset=%zu", offsetof(sqfs_opts, offset), 0},
            {"timeout=%u", offsetof(sqfs_opts, idle_timeout_secs), 0},
            FUSE_OPT_END
    };

    memset(&setup->sqfs_ll_ops, 0, sizeof(setup->sqfs_ll_ops));
    setup->sqfs_ll_ops.getattr = sqfs_ll_op_getattr;
    setup->sqfs_ll_ops.opendir = sqfs_ll_op_opendir;
    setup->sqfs_ll_ops.releasedir = sqfs_ll_op_releasedir;
    setup->sqfs_ll_ops.readdir = sqfs_ll_op_readdir;
    setup->sqfs_ll_ops.lookup = sqfs_ll_op_lookup;
    setup->sqfs_ll_ops.open = sqfs_ll_op_open;
    setup->sqfs_ll_ops.create = sqfs_ll_op_create;
    setup->sqfs_ll_ops.release = sqfs_ll_op_release;
    setup->sqfs_ll_ops.read = sqfs_ll_op_read;
    setup->sqfs_ll_ops.readlink = sqfs_ll_op_readlink;
    setup->sqfs_ll_ops.listxattr = sqfs_ll_op_listxattr;
    setup->sqfs_ll_ops.getxattr = sqfs_ll_op_getxattr;
    setup->sqfs_ll_ops.forget = sqfs_ll_op_forget;
    setup->sqfs_ll_ops.statfs = stfs_ll_op_statfs;

    /* PARSE ARGS */
    setup->args.argc = argc;
    setup->args.argv = argv;
    setup->args.allocated = 0;

    setup->opts.progname = argv[0];
    setup->opts.image = NULL;
    setup->opts.mountpoint = 0;
    setup->opts.offset = 0;
    setup->opts.idle_timeout_secs = 0;
    if (fuse_opt_parse(&setup->args, &setup->opts, fuse_opts, sqfs_opt_proc) == -1)
        sqfs_usage(argv[0], true);

#if FUSE_USE_VERSION >= 30
    if (fuse_parse_cmdline(&setup->args, &setup->fuse_cmdline_opts) != 0)
#else
        if (fuse_parse_cmdline(&args,
                           &fuse_cmdline_opts.mountpoint,
                           &fuse_cmdline_opts.mt,
                           &fuse_cmdline_opts.foreground) == -1)
#endif
        sqfs_usage(argv[0], true);
    if (setup->fuse_cmdline_opts.mountpoint == NULL)
        sqfs_usage(argv[0], true);

    return setup;
}


int start_fuse_worker(squashfuse_setup* setup) {
    int err;
    sqfs_ll* ll;

    /* OPEN FS */
    err = !(ll = sqfs_ll_open(setup->opts.image, setup->opts.offset));

    /* STARTUP FUSE */
    if (!err) {
        sqfs_ll_chan ch;
        err = -1;
        if (sqfs_ll_mount(
                &ch,
                setup->fuse_cmdline_opts.mountpoint,
                &setup->args,
                &setup->sqfs_ll_ops,
                sizeof(setup->sqfs_ll_ops),
                ll) == SQFS_OK) {

            if (fuse_set_signal_handlers(ch.session) != -1) {
                if (setup->opts.idle_timeout_secs) {
                    setup_idle_timeout(ch.session, setup->opts.idle_timeout_secs);
                }
                /* FIXME: multithreading */
                err = fuse_session_loop(ch.session);
                teardown_idle_timeout();
                fuse_remove_signal_handlers(ch.session);
            }

            sqfs_ll_destroy(ll);
            sqfs_ll_unmount(&ch, setup->fuse_cmdline_opts.mountpoint);
        }
    }
    fuse_opt_free_args(&setup->args);
    free(ll);
    free(setup->fuse_cmdline_opts.mountpoint);

    return -err;
}

squashfuse_setup* setup_squashfuse_foreground_execution(char* file, char* mount_point, size_t offset) {
    int argc = 5;
    char* argv[argc + 1];
    argv[0] = file;
    argv[1] = file;
    argv[2] = mount_point;
    argv[3] = "-o";
    argv[4] = malloc(sizeof(char) * 250);
    sprintf(argv[4], "ro,offset=%lu", offset);

    argv[argc] = NULL;

    squashfuse_setup* setup = init_squashfuse(argc, argv);
    return setup;
}

int mount_payload(char* file, char* mount_point, size_t offset) {
    squashfuse_setup* setup = setup_squashfuse_foreground_execution(file, mount_point, offset);
    return start_fuse_worker(setup);
}
