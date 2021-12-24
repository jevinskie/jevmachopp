#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>

extern "C" char **environ;

extern "C" int real_main(int argc, char **argv, char **env, const char **apple);

int main(int argc, char *const *argv) {
    (void)real_main; // force linker failure if missing
    const char *injlib = getenv("JEV_INJECTOR_LIB");
    assert(injlib);
    const auto set_inslib_res = setenv("DYLD_INSERT_LIBRARIES", injlib, 0);
    assert(!set_inslib_res);
    const auto set_injbin_res = setenv("JEV_INJECTOR_BIN", argv[0], 0);
    assert(!set_injbin_res);
    const auto execve_res = execve("/usr/bin/true", argv, environ);
    printf("execve_res: %d errno: %d err: %s\n", execve_res, errno, strerror(errno));
    return 1;
}