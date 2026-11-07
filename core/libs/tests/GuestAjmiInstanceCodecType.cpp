#include "prx/libSceAjmi/libSceAjmi.hpp"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <initializer_list>

namespace {

void Require(bool condition, const char* message) {
    if (condition) return;
    std::fprintf(stderr, "AJMI INSTANCE CODEC TYPE: %s\n", message);
    std::abort();
}

}

int main() {
    for (const std::uint32_t instance : {0u, 1u, 0x3fffu, 0x4000u, 0x4001u, 0x7fffu, 0x8000u, 0x12345678u, 0x7fffffffu, 0x80000000u, 0xffffffffu}) {
        Require(sceAjmiInstanceCodecType(instance) == static_cast<int>(instance >> 14), "the result is the instance shifted right by 14");
    }
    Require(sceAjmiInstanceCodecType(0xffffffffu) == 0x3ffff, "zeros are shifted in");
    std::puts("Ajmi instance codec type tests passed");
    return 0;
}
