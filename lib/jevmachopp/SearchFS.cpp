#include "jevmachopp/SearchFS.h"

#include <cerrno>
#include <cstring>
#include <sys/attr.h>
#include <sys/fsgetpath.h>
#include <unistd.h>

#include <thread_pool.hpp>

namespace SearchFS {

bool processVolume(const char *volume_path) {
    unsigned long num_match;
    unsigned int search_opts = 0;
    struct fssearchblock search_blk;
    struct attrlist return_list;
    struct searchstate search_state;

    const auto res = searchfs(volume_path, &search_blk, &num_match, 0, search_opts, &search_state);

    printf("res: %d errno: %d error: %s\n", res, errno, strerror(errno));

    return true;
}

} // namespace SearchFS
