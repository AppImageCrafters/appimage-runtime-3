#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

// squashfuse headers
#include "ll.h"
#include "fuseprivate.h"
#include "nonstd.h"

#include "commands.h"


void _init_squashfuse(struct fuse_args* args, sqfs_opts* opts, struct fuse_cmdline_opts* fuse_cmdline_opts,
                      struct fuse_lowlevel_ops* sqfs_ll_ops);


int _start_fuse_daemon(struct fuse_args* args, sqfs_opts* opts, struct fuse_cmdline_opts* fuse_cmdline_opts,
                       struct fuse_lowlevel_ops* sqfs_ll_ops);

void _init_squashfuse(struct fuse_args* args, sqfs_opts* opts, struct fuse_cmdline_opts* fuse_cmdline_opts,
                      struct fuse_lowlevel_ops* sqfs_ll_ops) {
    int argc = 7;
    char* argv[argc + 1];
    argv[0] = "/home/alexis/Workspace/third_party/curl-appimage/AppDir.sqfs";
    argv[1] = "/home/alexis/Workspace/third_party/curl-appimage/AppDir.sqfs";
    argv[2] = "/tmp/AppDir";
    argv[3] = "-o";
    argv[4] = "rw,offset=0";
    argv[5] = "-f";
    argv[6] = "-s";
    argv[argc] = NULL;
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
    memset(sqfs_ll_ops, 0, sizeof((*sqfs_ll_ops)));
    (*sqfs_ll_ops).getattr = sqfs_ll_op_getattr;
    (*sqfs_ll_ops).opendir = sqfs_ll_op_opendir;
    (*sqfs_ll_ops).releasedir = sqfs_ll_op_releasedir;
    (*sqfs_ll_ops).readdir = sqfs_ll_op_readdir;
    (*sqfs_ll_ops).lookup = sqfs_ll_op_lookup;
    (*sqfs_ll_ops).open = sqfs_ll_op_open;
    (*sqfs_ll_ops).create = sqfs_ll_op_create;
    (*sqfs_ll_ops).release = sqfs_ll_op_release;
    (*sqfs_ll_ops).read = sqfs_ll_op_read;
    (*sqfs_ll_ops).readlink = sqfs_ll_op_readlink;
    (*sqfs_ll_ops).listxattr = sqfs_ll_op_listxattr;
    (*sqfs_ll_ops).getxattr = sqfs_ll_op_getxattr;
    (*sqfs_ll_ops).forget = sqfs_ll_op_forget;
    (*sqfs_ll_ops).statfs = stfs_ll_op_statfs;

    /* PARSE ARGS */
    (*args).argc = argc;
    (*args).argv = argv;
    (*args).allocated = 0;

    (*opts).progname = argv[0];
    (*opts).image = NULL;
    (*opts).mountpoint = 0;
    (*opts).offset = 0;
    (*opts).idle_timeout_secs = 0;
    if (fuse_opt_parse(args, opts, fuse_opts, sqfs_opt_proc) == -1)
        sqfs_usage(argv[0], true);

#if FUSE_USE_VERSION >= 30
    if (fuse_parse_cmdline(args, fuse_cmdline_opts) != 0)
#else
        if (fuse_parse_cmdline(&args,
                           &fuse_cmdline_opts.mountpoint,
                           &fuse_cmdline_opts.mt,
                           &fuse_cmdline_opts.foreground) == -1)
#endif
        sqfs_usage(argv[0], true);
    if ((*fuse_cmdline_opts).mountpoint == NULL)
        sqfs_usage(argv[0], true);

    /* fuse_daemonize() will unconditionally clobber fds 0-2.
     *
     * If we get one of these file descriptors in sqfs_ll_open,
     * we're going to have a bad time. Just make sure that all
     * these fds are open before opening the image file, that way
     * we must get a different fd.
     */
    while (true) {
        int fd = open("/dev/null", O_RDONLY);
        if (fd == -1) {
            /* Can't open /dev/null, how bizarre! However,
             * fuse_deamonize won't clobber fds if it can't
             * open /dev/null either, so we ought to be OK.
             */
            break;
        }
        if (fd > 2) {
            /* fds 0-2 are now guaranteed to be open. */
            close(fd);
            break;
        }
    }
}


int _start_fuse_daemon(struct fuse_args* args, sqfs_opts* opts, struct fuse_cmdline_opts* fuse_cmdline_opts,
                       struct fuse_lowlevel_ops* sqfs_ll_ops) {
    int err;
    sqfs_ll* ll;

    /* OPEN FS */
    err = !(ll = sqfs_ll_open((*opts).image, (*opts).offset));

    /* STARTUP FUSE */
    if (!err) {
        sqfs_ll_chan ch;
        err = -1;
        if (sqfs_ll_mount(
                &ch,
                (*fuse_cmdline_opts).mountpoint,
                args,
                sqfs_ll_ops,
                sizeof((*sqfs_ll_ops)),
                ll) == SQFS_OK) {
            if (sqfs_ll_daemonize((*fuse_cmdline_opts).foreground) != -1) {
                if (fuse_set_signal_handlers(ch.session) != -1) {
                    if ((*opts).idle_timeout_secs) {
                        setup_idle_timeout(ch.session, (*opts).idle_timeout_secs);
                    }
                    /* FIXME: multithreading */
                    err = fuse_session_loop(ch.session);
                    teardown_idle_timeout();
                    fuse_remove_signal_handlers(ch.session);
                }
            }
            sqfs_ll_destroy(ll);
            sqfs_ll_unmount(&ch, (*fuse_cmdline_opts).mountpoint);
        }
    }
    fuse_opt_free_args(args);
    free(ll);
    free((*fuse_cmdline_opts).mountpoint);

    return -err;
}

int mount_payload() {
    struct fuse_args args;
    sqfs_opts opts;
    struct fuse_cmdline_opts fuse_cmdline_opts;
    struct fuse_lowlevel_ops sqfs_ll_ops;

    _init_squashfuse(&args, &opts, &fuse_cmdline_opts, &sqfs_ll_ops);
    return _start_fuse_daemon(&args, &opts, &fuse_cmdline_opts, &sqfs_ll_ops);
}

