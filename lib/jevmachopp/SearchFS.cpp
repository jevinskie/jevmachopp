#include "jevmachopp/SearchFS.h"

#include <cerrno>
#include <cstring>
#include <sys/attr.h>
#include <sys/fsgetpath.h>
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

#define ITEMS_PER_SEARCH 2

bool processVolume(const char *volume_path) {
    struct fssearchblock searchblk = {};
    struct attrlist attrlist = {};
    struct searchstate state;
    void *qap;
    unsigned long nummatches;
    unsigned int options;

    int i;
    int res;
    int vntype;
    off_t filebytes;

    qap = malloc(ITEMS_PER_SEARCH * sizeof(int));
    assert(qap);

    options = SRCHFS_START | SRCHFS_MATCHDIRS | SRCHFS_MATCHFILES;

    /* Search for items with uid != 0 */
    options |= SRCHFS_NEGATEPARAMS | SRCHFS_SKIPLINKS;
    searchblk.searchattrs.bitmapcount = ATTR_BIT_MAP_COUNT;
    searchblk.searchattrs.commonattr = 0;
    searchblk.searchparams1 = qap;
    searchblk.searchparams2 = qap;
    searchblk.sizeofsearchparams1 = sizeof(int);
    searchblk.sizeofsearchparams2 = sizeof(int);

    /* Ask for type, uid, gid and file bytes */
    searchblk.returnattrs = &attrlist;
    attrlist.bitmapcount = ATTR_BIT_MAP_COUNT;
    attrlist.commonattr = 0;
    attrlist.dirattr = 0;
    attrlist.fileattr = 0;

    /* Collect 2,500 items at a time (~60K) */
    searchblk.returnbuffer = qap;
    searchblk.returnbuffersize = ITEMS_PER_SEARCH * sizeof(int);
    searchblk.maxmatches = ITEMS_PER_SEARCH;

    for (;;) {
        nummatches = 0;
        res = searchfs(volume_path, &searchblk, &nummatches, 0, options, &state);
        printf("res: %d errno: %d error: %s\n", res, errno, strerror(errno));
        if (res && errno != EAGAIN && errno != EBUSY) {
            // fprintf(stderr, "%d \n", errno);
            // err(1, "searchfs %s", strerror(errno));
            break;
        }
        if (res == 0 && nummatches == 0)
            break; /* all done */
        options &= ~SRCHFS_START;

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
