#include "prx/libSceVideoOut/libSceVideoOut.hpp"

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {

void Require(bool value, const char* what) {
    if (value) return;
    std::fprintf(stderr, "failed: %s
", what);
    std::abort();
}

struct FlagCase {
    std::uint32_t flags;
    int expected;
};

struct S3dCase {
    std::uint64_t value;
    int expected;
};

using Mode = std::array<std::uint8_t, 0x40>;

Mode Filled(std::uint8_t fill) {
    Mode mode;
    mode.fill(fill);
    return mode;
}

constexpr FlagCase IsSystemDefaultCases[] = {
    {0x0u, 1},
    {0x1u, 0},
    {0x2u, 1},
    {0x3u, 0},
    {0x4u, 1},
    {0x5u, 0},
    {0x6u, 1},
    {0x7u, 0},
    {0x8u, 1},
    {0x9u, 0},
    {0xeu, 1},
    {0xfu, 0},
    {0x10u, 1},
    {0xfffffff8u, 1},
    {0xfffffff9u, 0},
    {0xfffffffdu, 0},
    {0xfffffffbu, 0},
    {0xffffffffu, 0},
};

constexpr FlagCase IsVrCases[] = {
    {0x0u, 0},
    {0x1u, 0},
    {0x2u, 1},
    {0x3u, 1},
    {0x4u, 0},
    {0x5u, 0},
    {0x6u, 1},
    {0x7u, 1},
    {0x8u, 0},
    {0x9u, 0},
    {0xeu, 1},
    {0xfu, 1},
    {0x10u, 0},
    {0xfffffff8u, 0},
    {0xfffffff9u, 0},
    {0xfffffffdu, 0},
    {0xfffffffbu, 1},
    {0xffffffffu, 1},
};

constexpr FlagCase IsVrNativeCases[] = {
    {0x0u, 0},
    {0x1u, 0},
    {0x2u, 1},
    {0x3u, 1},
    {0x4u, 0},
    {0x5u, 0},
    {0x6u, 0},
    {0x7u, 0},
    {0x8u, 0},
    {0x9u, 0},
    {0xeu, 0},
    {0xfu, 0},
    {0x10u, 0},
    {0xfffffff8u, 0},
    {0xfffffff9u, 0},
    {0xfffffffdu, 0},
    {0xfffffffbu, 1},
    {0xffffffffu, 0},
};

constexpr FlagCase IsVrLegacyCases[] = {
    {0x0u, 0},
    {0x1u, 0},
    {0x2u, 0},
    {0x3u, 0},
    {0x4u, 0},
    {0x5u, 0},
    {0x6u, 1},
    {0x7u, 1},
    {0x8u, 0},
    {0x9u, 0},
    {0xeu, 1},
    {0xfu, 1},
    {0x10u, 0},
    {0xfffffff8u, 0},
    {0xfffffff9u, 0},
    {0xfffffffdu, 0},
    {0xfffffffbu, 0},
    {0xffffffffu, 1},
};

constexpr S3dCase S3dCases[] = {
    {0x0ull, 0},
    {0x1ull, 0},
    {0x9full, 0},
    {0xa0ull, 1},
    {0xa1ull, 1},
    {0xa2ull, 0},
    {0x1000000a0ull, 0},
    {0x81ffffffull, 0},
    {0xffffffff81fffffeull, 0},
    {0xffffffff81ffffffull, 1},
    {0xffffffff82000000ull, 0},
    {0x7fffffff81ffffffull, 0},
    {0xffffffffffffffffull, 0},
};

constexpr std::uint8_t HdrValues[] = {12, 13, 14, 15, 16, 17};

constexpr std::uint8_t InitializedMode[0x20] = {0x20, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};

template <std::size_t N>
void CheckFlags(int (*query)(const void*), const FlagCase (&cases)[N], const char* what) {
    for (const FlagCase& c : cases) {
        for (const std::uint8_t fill : {std::uint8_t{0x00}, std::uint8_t{0xff}}) {
            Mode mode = Filled(fill);
            std::memcpy(mode.data() + 0x20, &c.flags, sizeof(c.flags));
            Require(query(mode.data()) == c.expected, what);
        }
    }
}

bool IsHdrValue(unsigned value) {
    for (const std::uint8_t hdr : HdrValues) {
        if (hdr == value) return true;
    }
    return false;
}

}

int main() {

    CheckFlags(sceVideoOutSysModeIsSystemDefault, IsSystemDefaultCases, "sceVideoOutSysModeIsSystemDefault answers as firmware 13.42 does for the word at 0x20");

    CheckFlags(sceVideoOutSysModeIsVr, IsVrCases, "sceVideoOutSysModeIsVr answers as firmware 13.42 does for the word at 0x20");

    CheckFlags(sceVideoOutSysModeIsVrNative, IsVrNativeCases, "sceVideoOutSysModeIsVrNative answers as firmware 13.42 does for the word at 0x20");

    CheckFlags(sceVideoOutSysModeIsVrLegacy, IsVrLegacyCases, "sceVideoOutSysModeIsVrLegacy answers as firmware 13.42 does for the word at 0x20");

    for (unsigned value = 0; value < 256; ++value) {
        for (const std::uint8_t fill : {std::uint8_t{0x00}, std::uint8_t{0xff}}) {
            Mode mode = Filled(fill);
            mode[6] = static_cast<std::uint8_t>(value);
            Require(sceVideoOutSysModeIsHdr(mode.data()) == (IsHdrValue(value) ? 1 : 0), "sceVideoOutSysModeIsHdr answers as firmware 13.42 does for the byte at 6");
        }
    }

    for (const S3dCase& c : S3dCases) {
        for (const std::uint8_t fill : {std::uint8_t{0x00}, std::uint8_t{0xff}}) {
            Mode mode = Filled(fill);
            std::memcpy(mode.data() + 0x10, &c.value, sizeof(c.value));
            Require(sceVideoOutSysIsModeS3d(mode.data()) == c.expected, "sceVideoOutSysIsModeS3d answers as firmware 13.42 does for the word at 0x10");
        }
    }

    for (const std::uint8_t fill : {std::uint8_t{0x00}, std::uint8_t{0x5a}}) {
        Mode mode = Filled(fill);
        sceVideoOutSysInitMode(mode.data());
        Require(std::memcmp(mode.data(), InitializedMode, sizeof(InitializedMode)) == 0, "sceVideoOutSysInitMode writes the size and fills the rest of 32 bytes with 0xff");
        for (std::size_t i = sizeof(InitializedMode); i < mode.size(); ++i) Require(mode[i] == fill, "sceVideoOutSysInitMode leaves the bytes past 32 untouched");
    }

    std::puts("guest_video_out_sys_mode: all checks passed");
    return 0;
}
