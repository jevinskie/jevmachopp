#pragma once

template <typename E> constexpr auto to_underlying_int(E e) noexcept {
    return +(static_cast<std::underlying_type_t<E>>(e));
}
