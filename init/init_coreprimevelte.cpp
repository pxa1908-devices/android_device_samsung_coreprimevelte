/*
   Copyright (c) 2016, The Linux Foundation. All rights reserved.
   Copyright (c) 2017-2018, The LineageOS Project. All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

#include <cutils/properties.h>

#include "vendor_init.h"

#if __has_include(<android-base/logging.h>)
#include <android-base/logging.h>
#include <android-base/properties.h>
#define LOG_INFO_INIT(msg) LOG(INFO) << "init: " << msg
#else
#include "log.h"
#include "property_service.h"
#include "util.h"
#define LOG_INFO_INIT(msg) INFO("init: %s\n", msg.c_str())
#endif

#include "init_pxa1908.h"

static std::string get_prop(const std::string& key)
{
#if __has_include(<android-base/properties.h>)
    return android::base::GetProperty(key, "");
#else
    char prop_val[PROP_VALUE_MAX];
    property_get(key.c_str(), prop_val);
    return std::string(prop_val);
#endif
}

static void set_prop(const char* key, const char* value)
{
#if __has_include(<android-base/properties.h>)
    android::base::SetProperty(key, value);
#else
    property_set(key, value);
#endif
}

void set_rild_libpath(char const *variant)
{
    char libpath[512];

    strcpy(libpath, "/system/lib/libsec-ril");
    strcat(libpath, variant);
    strcat(libpath,".so");
    property_override("rild.libpath", libpath);
}

void cdma_properties(char const *operator_alpha,
        char const *operator_numeric,
        char const *default_network,
        char const *rild_lib_variant)
{
    /* Dynamic CDMA Properties */
    set_prop("ro.cdma.home.operator.alpha", operator_alpha);
    set_prop("ro.cdma.home.operator.numeric", operator_numeric);
    set_prop("ro.telephony.default_network", default_network);
    set_rild_libpath(rild_lib_variant);

    /* Static CDMA Properties */
    set_prop("ril.subscription.types", "NV,RUIM");
    set_prop("ro.telephony.default_cdma_sub", "0");
    set_prop("telephony.lteOnCdmaDevice", "1");
}

void lte_properties(char const *rild_lib_variant)
{
    set_rild_libpath(rild_lib_variant);

    set_prop("ro.telephony.default_network", "9");
    set_prop("telephony.lteOnGsmDevice", "1");
}

void gsm_properties(char const *rild_lib_variant)
{
    set_rild_libpath(rild_lib_variant);

    set_prop("ro.telephony.default_network", "3");
    set_prop("telephony.lteOnCdmaDevice", "0");
}

void init_target_properties()
{
    std::string platform = get_prop("ro.board.platform");
    if (platform != ANDROID_TARGET)
        return;

    std::string bootloader = get_prop("ro.bootloader");

    if( bootloader.find("G361F") == 0 )
    {
        property_override("ro.build.fingerprint", "samsung/coreprimeveltexx/coreprimevelte:5.1.1/LMY48B/G361FXXU1AOL1:user/release-keys");
        property_override("ro.build.description", "coreprimeveltexx-user 5.1.1 LMY48B G361FXXU1AOL1 release-keys");
        property_override("ro.product.model", "SM-G361F");
        property_override("ro.product.device", "coreprimevelte");
        property_override("ro.telephony.ril_class", "SamsungPXA1908RIL");
        lte_properties("");
    }

    std::string device = get_prop("ro.product.device");
    std::string log_msg = "Found bootloader id " + bootloader + " setting build properties for " + device + " device";
    LOG_INFO_INIT(log_msg);
}
