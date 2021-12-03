#include "jevmachopp/SearchFS.h"

#include <cerrno>
#include <cstring>
#include <limits.h>
#include <sys/attr.h>
#include <sys/fsgetpath.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include <thread_pool.hpp>

namespace SearchFS {

namespace Documentation {

// https://github.com/apple-opensource/diskdev_cmds/blob/5edd85296237102b4cf721025103875f9eefb9c0/quotacheck.tproj/hfs_quotacheck.c#L92-L169

struct fssearchblock {
    struct attrlist *returnattrs;
    void *returnbuffer;
    size_t returnbuffersize;
    unsigned int maxmatches;
    struct timeval timelimit;
    void *searchparams1;
    size_t sizeofsearchparams1;
    void *searchparams2;
    size_t sizeofsearchparams2;
    struct attrlist searchattrs;
};
} // namespace Documentation

#define ITEMS_PER_SEARCH 4

struct sz_attr_buf {
    uint32_t buf_sz;
    off_t sz;
} __attribute__((aligned(4), packed));

struct res_attr_buf {
    uint32_t buf_sz;
    fsid_t fsid;
    uint64_t oid;
} __attribute__((aligned(4), packed));

static_assert_size_is(res_attr_buf, 0x14);

static void foo(void) {
    struct statfs statfs_buf = {};
    const auto statfs_res = statfs("/Volumes/smolfs/LEB128.h", &statfs_buf);
    printf("statfs_res: %d fsid: 0x%016llx\n", statfs_res, *(uint64_t *)&statfs_buf.f_fsid);

    struct stat stat_buf = {};
    const auto stat_res = stat("/Volumes/smolfs/LEB128.h", &stat_buf);
    printf("stat_res: %d st_ino: 0x%016llx\n", stat_res, stat_buf.st_ino);

    char path[PATH_MAX] = {};
    const auto getpath_res = fsgetpath(path, sizeof(path), &statfs_buf.f_fsid, stat_buf.st_ino);
    printf("getpath_res: %zd path: %s\n", getpath_res, path);
}

bool processVolume(const char *volume_path) {
    struct fssearchblock searchblk = {};
    struct attrlist resattr = {};
    struct searchstate state = {};
    res_attr_buf *qap = nullptr;
    unsigned long num_matches = 0;
    uint32_t options = 0;
    sz_attr_buf search_attrs = {sizeof(sz_attr_buf), 4};
    sz_attr_buf search_attrs_max = {sizeof(sz_attr_buf), INT64_MAX};

    foo();

    int res = -1;

    qap = (res_attr_buf *)malloc(ITEMS_PER_SEARCH * sizeof(res_attr_buf));
    assert(qap);
    memset(qap, 0, ITEMS_PER_SEARCH * sizeof(res_attr_buf));

    options = SRCHFS_START | SRCHFS_MATCHFILES;

    options |= SRCHFS_SKIPLINKS;
    searchblk.searchattrs.bitmapcount = ATTR_BIT_MAP_COUNT;
    searchblk.searchattrs.fileattr = ATTR_FILE_DATALENGTH;
    searchblk.searchparams1 = &search_attrs;
    searchblk.sizeofsearchparams1 = sizeof(search_attrs);
    searchblk.searchparams2 = &search_attrs_max;
    searchblk.sizeofsearchparams2 = sizeof(search_attrs_max);

    /* Ask for type, uid, gid and file bytes */
    searchblk.returnattrs = &resattr;
    resattr.bitmapcount = ATTR_BIT_MAP_COUNT;
    resattr.commonattr = ATTR_CMN_FSID | ATTR_CMN_OBJID;

    /* Collect 2,500 items at a time (~60K) */
    searchblk.returnbuffer = qap;
    searchblk.returnbuffersize = ITEMS_PER_SEARCH * sizeof(res_attr_buf);
    searchblk.maxmatches = ITEMS_PER_SEARCH;

    int j = 0;
    for (;;) {
        num_matches = 0;
        res = searchfs(volume_path, &searchblk, &num_matches, 0, options, &state);

        if (res && errno != EAGAIN && errno != EBUSY) {
            // fprintf(stderr, "%d \n", errno);
            // err(1, "searchfs %s", strerror(errno));
            break;
        }
        if (res == 0 && num_matches == 0)
            break; /* all done */
        options &= ~SRCHFS_START;

        if (j++ > 100) {
            break;
        }
        // printf("res: %d num_matches: %lu errno: %d error: %s\n", res, num_matches, errno,
        // strerror(errno));

        printf("sz: 0x%08x fsid: 0x%016llx oid: 0x%016llx\n", qap[0].buf_sz,
               *(uint64_t *)&qap[0].fsid, qap[0].oid);

        char path[PATH_MAX] = {};
        const auto getpath_res = fsgetpath(path, sizeof(path), &qap[0].fsid, qap[0].oid);
        printf("getpath_res: %zd path: %s\n", getpath_res, path);

        // for (i = 0; i < nummatches; ++i) {
        //     vntype = qap[i].qa_type;
        //     filebytes = (vntype == VDIR) ? 0 : qap[i].qa_bytes;

        //     if (qnp->flags & HASGRP) {
        //         // fup = addid(qap[i].qa_gid, GRPQUOTA);
        //         fup->fu_curinodes++;
        //         if (vntype == VREG || vntype == VDIR || vntype == VLNK)
        //             fup->fu_curbytes += filebytes;
        //     }
        //     if (qnp->flags & HASUSR) {
        //         fup = addid(qap[i].qa_uid, USRQUOTA);
        //         fup->fu_curinodes++;
        //         if (vntype == VREG || vntype == VDIR || vntype == VLNK)
        //             fup->fu_curbytes += filebytes;
        //     }
        // }
    }

    free(qap);

    return true;
}

} // namespace SearchFS
