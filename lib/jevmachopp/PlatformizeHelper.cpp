#include <cassert>
#include <cstdlib>
#include <dlfcn.h>

typedef int (*main_t)(int argc, char *const *argv, char *const *env, char *const *apple);

__attribute((constructor)) void load_and_run_injbin(int argc, char *const *argv, char *const *env,
                                                    char *const *apple) {
    const char *injbin = getenv("JEV_INJECTOR_BIN");
    assert(injbin);
    void *bin_handle = dlopen(injbin, 0);
    assert(bin_handle);
    const auto real_main_fptr = (main_t)dlsym(bin_handle, "real_main");
    assert(real_main_fptr);
    const auto real_main_res = real_main_fptr(argc, argv, env, apple);
    exit(real_main_res);
}
