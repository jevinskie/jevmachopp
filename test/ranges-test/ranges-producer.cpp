#include "ranges-producer.h"

int foo(void) {
    return 42;
}

std::list<Version> makeVersions() {
    return {{0, 0, 0}, {1, 0, 0}, {2, 0, 0}, {3, 2, 1}, {4, 40, 400}, {5, 0, 0}, {243, 42, 7}};
}
