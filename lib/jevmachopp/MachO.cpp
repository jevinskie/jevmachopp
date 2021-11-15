//
//  MachO.cpp
//  macho-remove-section
//
//  Created by Jevin Sweval on 5/23/13.
//  Copyright (c) 2013 Arxan. All rights reserved.
//

#include "jevmachopp/MachO.h"
#include <algorithm>
#include <assert.h>
#include <mach-o/loader.h>
#include <numeric>

