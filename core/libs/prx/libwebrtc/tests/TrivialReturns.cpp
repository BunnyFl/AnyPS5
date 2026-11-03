#include "prx/libwebrtc/libwebrtc.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <source_location>

namespace {

void Require(bool condition, std::source_location location = std::source_location::current()) {
    if (condition) return;
    std::fprintf(stderr, "libwebrtc trivial returns check failed at line %u\n", location.line());
    std::abort();
}

std::array<unsigned char, 64> Filled() {
    std::array<unsigned char, 64> bytes{};
    std::memset(bytes.data(), 0x5A, bytes.size());
    return bytes;
}

bool Unchanged(const std::array<unsigned char, 64>& bytes) {
    for (const auto byte : bytes) {
        if (byte != 0x5A) return false;
    }
    return true;
}

} // namespace

int main() {
    using Destructor = void (APS5_VABI *)(void*);
    for (const Destructor destructor : {
             _ZN3rtc10PacketInfoD1Ev_nid_postfix,
             _ZN6webrtc12VideoEncoder21RateControlParametersD1Ev_nid_postfix,
             _ZN6webrtc13PacketOptionsD1Ev_nid_postfix,
             _ZN6webrtc19SmoothingFilterImplD1Ev_nid_postfix,
             _ZN6webrtc23AudioNetworkAdaptorImpl6ConfigD1Ev_nid_postfix,
             _ZN6webrtc25AudioEncoderRuntimeConfigD1Ev_nid_postfix,
             _ZN6webrtc4rtcp3PliD1Ev_nid_postfix,
             _ZN6webrtc8RmsLevelD1Ev_nid_postfix,
         }) {
        auto object = Filled();
        destructor(object.data());
        Require(Unchanged(object));
    }

    auto encoder = Filled();
    _ZN6webrtc12AudioEncoder45OnReceivedUplinkRecoverablePacketLossFractionEf_nid_postfix(encoder.data(), 0.25f);
    Require(Unchanged(encoder));

    auto videoEncoder = Filled();
    auto controller = Filled();
    _ZN6webrtc12VideoEncoder24SetFecControllerOverrideEPNS_21FecControllerOverrideE_nid_postfix(videoEncoder.data(), controller.data());
    _ZN6webrtc12VideoEncoder24SetFecControllerOverrideEPNS_21FecControllerOverrideE_nid_postfix(videoEncoder.data(), nullptr);
    Require(Unchanged(videoEncoder));
    Require(Unchanged(controller));

    auto manager = Filled();
    Require(_ZNK3rtc14NetworkManager16GetMdnsResponderEv_nid_postfix(manager.data()) == nullptr);
    Require(_ZThn8_NK3rtc14NetworkManager16GetMdnsResponderEv_nid_postfix(manager.data() + 8) == nullptr);
    Require(Unchanged(manager));

    auto buffer = Filled();
    Require(_ZNK6webrtc16VideoFrameBuffer7GetI420Ev_nid_postfix(buffer.data()) == nullptr);
    Require(Unchanged(buffer));

    Require(libwebrtcUnknown37() == 0);
}
