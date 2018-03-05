//
//  kern_ngfx.cpp
//  VegaGraphicsFixup based on NvidiaGraphicsFixup
//
//  Copyright © 2017 lvs1974. All rights reserved.
//  Copyright © 2018 JYA. All rights reserved.
//

#include <Headers/kern_api.hpp>
#include <Headers/kern_util.hpp>
#include <Headers/kern_iokit.hpp>

#include "kern_config.hpp"
#include "kern_ngfx.hpp"


static const char *kextAGDPolicy[] { "/System/Library/Extensions/AppleGraphicsControl.kext/Contents/PlugIns/AppleGraphicsDevicePolicy.kext/Contents/MacOS/AppleGraphicsDevicePolicy" };
static const char *kextAGDPolicyId { "com.apple.driver.AppleGraphicsDevicePolicy" };

static const char *kextGeForce[] { "/System/Library/Extensions/GeForce.kext/Contents/MacOS/GeForce" };
static const char *kextGeForceId { "com.apple.GeForce" };

static KernelPatcher::KextInfo kextList[] {
    { kextAGDPolicyId,      kextAGDPolicy,   arrsize(kextAGDPolicy),  {true}, {}, KernelPatcher::KextInfo::Unloaded },
    { kextGeForceId,        kextGeForce,     arrsize(kextGeForce),    {},     {}, KernelPatcher::KextInfo::Unloaded },
};

static size_t kextListSize {arrsize(kextList)};

// Only used in apple-driven callbacks
static NGFX *callbackNGFX = nullptr;


bool NGFX::init() {
	LiluAPI::Error error = lilu.onKextLoad(kextList, kextListSize,
        [](void *user, KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
            callbackNGFX = static_cast<NGFX *>(user);
            callbackNGFX->processKext(patcher, index, address, size);
        }, this);
	
	if (error != LiluAPI::Error::NoError) {
		SYSLOG("ngfx", "failed to register onKextLoad method %d", error);
		return false;
	}
	
	return true;
}

void NGFX::deinit() {
}

void NGFX::processKext(KernelPatcher &patcher, size_t index, mach_vm_address_t address, size_t size) {
	if (progressState != ProcessingState::EverythingDone) {
		for (size_t i = 0; i < kextListSize; i++) {
			if (kextList[i].loadIndex == index) {
                if (!(progressState & ProcessingState::GraphicsDevicePolicyPatched) && !strcmp(kextList[i].id, kextAGDPolicyId))
                {
                    DBGLOG("ngfx", "found %s", kextAGDPolicyId);

                    const uint8_t find[]    = "board-id";
                    const uint8_t replace[] = "board-ix";
                    KextPatch kext_patch {
                        {&kextList[i], find, replace, strlen((const char*)find), 1},
                        KernelVersion::MountainLion, KernelPatcher::KernelAny
                    };

                    applyPatches(patcher, index, &kext_patch, 1, "pikera");

                    progressState |= ProcessingState::GraphicsDevicePolicyPatched;
                }
                else if (!(progressState & ProcessingState::GeForceRouted) && !strcmp(kextList[i].id, kextGeForceId))
                {
                    DBGLOG("ngfx", "found %s", kextGeForceId);
                    auto method_address = patcher.solveSymbol(index, "__ZN13nvAccelerator18SetAccelPropertiesEv");
                    if (method_address) {
                        DBGLOG("ngfx", "obtained __ZN13nvAccelerator18SetAccelPropertiesEv");
                        patcher.clearError();
                        orgSetAccelProperties = reinterpret_cast<t_set_accel_properties>(patcher.routeFunction(method_address, reinterpret_cast<mach_vm_address_t>(nvAccelerator_SetAccelProperties), true));
                        if (patcher.getError() == KernelPatcher::Error::NoError) {
                            DBGLOG("ngfx", "routed __ZN13nvAccelerator18SetAccelPropertiesEv");
                        } else {
                            SYSLOG("ngfx", "failed to route __ZN13nvAccelerator18SetAccelPropertiesEv");
                        }
                    } else {
                        SYSLOG("ngfx", "failed to resolve __ZN13nvAccelerator18SetAccelPropertiesEv");
                    }

                    progressState |= ProcessingState::GeForceRouted;
                }
 			}
		}
	}
    
	// Ignore all the errors for other processors
	patcher.clearError();
}

void NGFX::nvAccelerator_SetAccelProperties(IOService* that)
{
    DBGLOG("ngfx", "SetAccelProperties is called");
}


void NGFX::applyPatches(KernelPatcher &patcher, size_t index, const KextPatch *patches, size_t patchNum, const char* name) {
    DBGLOG("ngfx", "applying patch '%s' for %zu kext", name, index);
    for (size_t p = 0; p < patchNum; p++) {
        auto &patch = patches[p];
        if (patch.patch.kext->loadIndex == index) {
            if (patcher.compatibleKernel(patch.minKernel, patch.maxKernel)) {
                DBGLOG("ngfx", "applying %zu patch for %zu kext", p, index);
                patcher.applyLookupPatch(&patch.patch);
                // Do not really care for the errors for now
                patcher.clearError();
            }
        }
    }
}

