#ifndef CORE_LIBS_PRX_LIBSCEVIDEOUOUT_INCLUDE_SYSMODE_HPP
#define CORE_LIBS_PRX_LIBSCEVIDEOUOUT_INCLUDE_SYSMODE_HPP

#include "SceTypes.hpp"

extern "C" {

void APS5_VABI sceVideoOutSysInitMode(void* mode);
int APS5_VABI sceVideoOutSysIsModeS3d(const void* mode);
int APS5_VABI sceVideoOutSysModeIsHdr(const void* mode);
int APS5_VABI sceVideoOutSysModeIsSystemDefault(const void* mode);
int APS5_VABI sceVideoOutSysModeIsVr(const void* mode);
int APS5_VABI sceVideoOutSysModeIsVrLegacy(const void* mode);
int APS5_VABI sceVideoOutSysModeIsVrNative(const void* mode);

}

#endif
