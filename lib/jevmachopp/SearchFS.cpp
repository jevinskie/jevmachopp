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

#define ITEMS_PER_SEARCH 4096 * 128
// #define ITEMS_PER_SEARCH 4

struct sz_attr_t {
    uint32_t buf_sz;
    uint64_t sz;
} __attribute__((aligned(4), packed));

struct oid_attr_t {
    uint32_t buf_sz;
    uint64_t oid;
} __attribute__((aligned(4), packed));

bool files_larger_than(const char *volume_path, std::size_t min_sz) {
    struct fssearchblock searchblk = {};
    struct attrlist resattrlist = {};
    struct searchstate state = {};
    // oid_attr_t res_buf[ITEMS_PER_SEARCH];
    uint8_t res_buf_raw[ITEMS_PER_SEARCH * sizeof(oid_attr_t) + 1];
    oid_attr_t *res_buf = (oid_attr_t *)res_buf_raw;
    unsigned long num_matches = 0;
    uint32_t options = 0;
    sz_attr_t search_attrs = {sizeof(sz_attr_t), min_sz};
    sz_attr_t search_attrs_max = {sizeof(sz_attr_t), UINT64_MAX};
    struct statfs statfs_buf = {};
    char path[PATH_MAX];
    int res = -1;

    auto statfs_res = statfs(volume_path, &statfs_buf);
    assert(!statfs_res);

    options = SRCHFS_START | SRCHFS_MATCHFILES;
    searchblk.searchattrs.bitmapcount = ATTR_BIT_MAP_COUNT;
    searchblk.searchattrs.fileattr = ATTR_FILE_DATALENGTH;
    searchblk.searchparams1 = &search_attrs;
    searchblk.sizeofsearchparams1 = sizeof(search_attrs);
    searchblk.searchparams2 = &search_attrs_max;
    searchblk.sizeofsearchparams2 = sizeof(search_attrs_max);

    /* Ask for type, uid, gid and file bytes */
    searchblk.returnattrs = &resattrlist;
    resattrlist.bitmapcount = ATTR_BIT_MAP_COUNT;
    resattrlist.commonattr = ATTR_CMN_OBJID;

    /* Collect 2,500 items at a time (~60K) */
    searchblk.returnbuffer = res_buf;
    searchblk.returnbuffersize = sizeof(res_buf_raw);
    searchblk.maxmatches = ITEMS_PER_SEARCH;

    for (;;) {
        num_matches = 0;
        res = searchfs(volume_path, &searchblk, &num_matches, 0x08000103, options, &state);

        if (res && !(errno != EAGAIN || errno != EBUSY)) {
            fprintf(stderr, "%d errno: %s\n", errno, strerror(errno));
            assert(0);
            break;
        }
        if (res && errno == EBUSY) {
            assert(!"FIXME handle restart");
        }
        // EAGAIN or res == 0 and last iteration
        options &= ~SRCHFS_START;

        printf("res: %d num_matches: %lu\n", res, num_matches);
        for (unsigned long i = 0; i < num_matches; ++i) {
            const auto getpath_res =
                fsgetpath(path, sizeof(path), &statfs_buf.f_fsid, res_buf[i].oid);
            assert(getpath_res);
            printf("path: %s\n", path);
        }

        if (res == 0) {
            break;
        }
    }

    return true;
}

} // namespace SearchFS
