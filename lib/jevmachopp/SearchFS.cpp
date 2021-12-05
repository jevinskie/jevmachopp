#include "jevmachopp/SearchFS.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <limits.h>
#include <sys/attr.h>
#include <sys/fsgetpath.h>
#include <sys/mount.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <unistd.h>

#include <thread_pool.hpp>

extern "C" int openbyid_np(void *fsid, void *objid, int flags);

namespace SearchFS {

// constexpr auto ITEMS_PER_SEARCH = 4096;
// constexpr auto ITEMS_PER_SEARCH = 128;
constexpr auto ITEMS_PER_SEARCH = 256;

struct sz_attr_t {
    uint32_t buf_sz;
    uint64_t sz;
} __attribute__((aligned(4), packed));

struct oid_attr_t {
    uint32_t buf_sz;
    ino_t ino;
} __attribute__((aligned(4), packed));

uint64_t num_files;

void print_result(std::string path) {
    // printf("path: %s\n", path.data());
    puts(path.data());
}

bool process_file(thread_pool *pool, uint64_t fsid, uint64_t ino) {
    ++num_files;
    // bool process_file(uint64_t fsid, uint64_t ino) {
    char path_cstr[PATH_MAX];
    uint32_t found_magic;
    // printf("fsid: 0x%016llx ino: 0x%016llx\n", fsid, ino);
    // const auto getpath_res = fsgetpath(path_cstr, sizeof(path_cstr), (fsid_t*)&fsid, ino);
    // assert(getpath_res);
    // pool->submit(print_result, std::move(std::string{path_cstr}));
    // printf("path: %s\n", path_cstr);
    // const auto getpath_res = fsgetpath(path_cstr, sizeof(path_cstr), (fsid_t *)&fsid, ino);
    // assert(getpath_res);
    // const auto fd = open(path_cstr, O_RDONLY);
    const auto fd = openbyid_np(&fsid, &ino, O_RDONLY);
    if (fd < 0) {
        const auto getpath_err_res = fsgetpath(path_cstr, sizeof(path_cstr), (fsid_t *)&fsid, ino);
        assert(getpath_err_res);
        printf("failed to open path: %s errno: %d err: %s\n", path_cstr, errno, strerror(errno));
    } else {
        // const auto read_res = read(fd, &found_magic, 4);
        // if (read_res != 4) {
        //     const auto getpath_err_res = fsgetpath(path_cstr, sizeof(path_cstr), (fsid_t *)&fsid,
        //     ino); assert(getpath_err_res); printf("failed to read 4 bytes from path: %s errno: %d
        //     err: %s\n", path_cstr, errno,
        //            strerror(errno));
        // }
        close(fd);
    }
    return true;
}

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
    int res = -1;
    thread_pool pool;
    uint64_t fsid_raw;

    printf("running on %u threads\n", pool.get_thread_count());

    auto statfs_res = statfs(volume_path, &statfs_buf);
    assert(!statfs_res);
    memcpy(&fsid_raw, &statfs_buf.f_fsid, sizeof(fsid_raw));

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

        // printf("res: %d num_matches: %lu\n", res, num_matches);
        for (unsigned long i = 0; i < num_matches; ++i) {
            pool.submit(process_file, &pool, fsid_raw, res_buf[i].ino);
            // pool.submit(process_file, fsid_raw, res_buf[i].ino);
        }

        if (res == 0) {
            break;
        }
    }

    pool.wait_for_tasks();
    printf("num files: %llu\n", num_files);

    return true;
}

} // namespace SearchFS
