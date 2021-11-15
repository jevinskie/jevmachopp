#include "jevmachopp/LoadSubCommand.h"
#include "jevmachopp/LoadCommand.h"
#include "jevmachopp/LoadCommandType.h"

const LoadCommand *LoadSubCommand::loadCommand() const {
    return (const LoadCommand *)((uintptr_t)this - sizeof(LoadCommand));
}

const SubCommandVariant LoadSubCommand::get() const {
    switch (LoadCommand().cmd) {
    case LoadCommandType::ENCRYPTION_INFO:
        return SubCommandVariant{(const EncryptionInfoCommand *)this};
    case LoadCommandType::SEGMENT_64:
        return SubCommandVariant{(const SegmentCommand *)this};
    case LoadCommandType::UUID:
        return SubCommandVariant{(const UUIDCommand *)this};
    default:
        return SubCommandVariant{(const UnknownCommand *)this};
    }
}

template <class> inline constexpr bool always_false_v = false;

template <> struct fmt::formatter<LoadSubCommand> {

    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) {
        return ctx.begin();
    }

    template <typename FormatContext> auto format(LoadSubCommand const &lsc, FormatContext &ctx) {
        decltype(fmt::format_to(ctx.out(), "{}", lsc.get())) res;
        std::visit(
            [=](auto &&o) {
                using T = std::decay_t<decltype(o)>;
                if constexpr (std::is_same_v<T, int>)
                    res = fmt::format_to(ctx.out(), "{}", o);
                else if constexpr (std::is_same_v<T, long>)
                    res = fmt::format_to(ctx.out(), "{}", o);
                else if constexpr (std::is_same_v<T, double>)
                    res = fmt::format_to(ctx.out(), "{}", o);
                else if constexpr (std::is_same_v<T, std::string>)
                    res = fmt::format_to(ctx.out(), "{}", o);
                else
                    static_assert(always_false_v<T>, "non-exhaustive visitor!");
            },
            lsc.get());
        return res;
    }
};