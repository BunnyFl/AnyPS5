#include "prx/libSceVideoOut/include/SysMode.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <stdexcept>

namespace {

constexpr std::size_t InitializedSize = 0x20;
constexpr std::size_t HdrByteOffset = 6;
constexpr std::size_t S3dWordOffset = 0x10;
constexpr std::size_t FlagWordOffset = 0x20;
constexpr std::uint32_t NotSystemDefaultBit = 1u << 0;
constexpr std::uint32_t VrBit = 1u << 1;
constexpr std::uint32_t VrLegacyBit = 1u << 2;
constexpr std::uint8_t FirstHdrValue = 12;
constexpr std::uint8_t HdrValueCount = 6;
constexpr std::uint64_t S3dModes[] = {0xa0, 0xa1, 0xffffffff81ffffffull};

const std::uint8_t* Bytes(const void* mode) {
    if (mode == nullptr) throw std::logic_error("sceVideoOutSys mode: a null mode faults in firmware 13.42");
    return static_cast<const std::uint8_t*>(mode);
}

std::uint32_t Flags(const void* mode) {
    std::uint32_t flags;
    std::memcpy(&flags, Bytes(mode) + FlagWordOffset, sizeof(flags));
    return flags;
}

}

extern "C" {

void APS5_VABI sceVideoOutSysInitMode(void* mode) {
    if (mode == nullptr) throw std::logic_error("sceVideoOutSysInitMode: a null mode faults in firmware 13.42");
    auto* bytes = static_cast<std::uint8_t*>(mode);
    std::memset(bytes, 0xff, InitializedSize);
    const std::uint32_t size = InitializedSize;
    std::memcpy(bytes, &size, sizeof(size));
}

int APS5_VABI sceVideoOutSysIsModeS3d(const void* mode) {
    std::uint64_t value;
    std::memcpy(&value, Bytes(mode) + S3dWordOffset, sizeof(value));
    for (const std::uint64_t s3d : S3dModes) {
        if (value == s3d) return 1;
    }
    return 0;
}

int APS5_VABI sceVideoOutSysModeIsHdr(const void* mode) {
    const std::uint8_t value = Bytes(mode)[HdrByteOffset];
    return value >= FirstHdrValue && value < FirstHdrValue + HdrValueCount ? 1 : 0;
}

int APS5_VABI sceVideoOutSysModeIsSystemDefault(const void* mode) {
    return (Flags(mode) & NotSystemDefaultBit) == 0 ? 1 : 0;
}

int APS5_VABI sceVideoOutSysModeIsVr(const void* mode) {
    return (Flags(mode) & VrBit) != 0 ? 1 : 0;
}

int APS5_VABI sceVideoOutSysModeIsVrLegacy(const void* mode) {
    return (Flags(mode) & (VrBit | VrLegacyBit)) == (VrBit | VrLegacyBit) ? 1 : 0;
}

int APS5_VABI sceVideoOutSysModeIsVrNative(const void* mode) {
    return (Flags(mode) & (VrBit | VrLegacyBit)) == VrBit ? 1 : 0;
}

}
