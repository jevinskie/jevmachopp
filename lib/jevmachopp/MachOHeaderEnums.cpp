#include "jevmachopp/MachOHeaderEnums.h"

std::experimental::fixed_capacity_vector<std::string_view, 32> to_strings(MachOFlags const &flags) {
    decltype(to_strings(flags)) res;

    for (size_t i = 0, e = MachOFlags_traits::size; i < e; ++i) {
        auto i_enum = MachOFlags_traits::from_index_or_default(i, MachOFlags::NOUNDEFS);
        if (as_unsigned(i_enum) & as_unsigned(flags)) {
            res.emplace_back(MachOFlags_traits::to_string_or_empty(i_enum));
        }
    }

    return res;
}
