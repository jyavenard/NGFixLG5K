//
//  kern_start.cpp
//  VegaGraphicsFixup based on NvidiaGraphicsFixup
//
//  Copyright © 2017 lvs1974. All rights reserved.
//  Copyright © 2018 JYA. All rights reserved.
//

#include <Headers/plugin_start.hpp>
#include <Headers/kern_api.hpp>

#include "kern_config.hpp"
#include "kern_ngfx.hpp"

static NGFX ngfx;

const char* bootarg[] { };

PluginConfiguration ADDPR(config) {
	xStringify(PRODUCT_NAME),
    parseModuleVersion(xStringify(MODULE_VERSION)),
    LiluAPI::AllowNormal | LiluAPI::AllowInstallerRecovery,
	bootarg,
	0,
	bootarg,
	0,
	bootarg,
	0,
	KernelVersion::MountainLion,
	KernelVersion::HighSierra,
	[]() {
		ngfx.init();
	}
};





