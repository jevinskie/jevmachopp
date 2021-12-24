#pragma once

#include "jevmachopp/Common.h"

class Section;

class CallStubs {
public:
    const Section &sect;

public:
    CallStubs(const Section &stubSect) : sect(stubSect) {}
};
