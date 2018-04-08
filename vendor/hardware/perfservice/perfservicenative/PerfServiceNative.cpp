/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */


#define LOG_TAG "PerfService"
#define ATRACE_TAG ATRACE_TAG_PERF

#include "utils/Log.h"
#include "PerfServiceNative.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <unistd.h>
#include <utils/String16.h>

#include <cutils/log.h>
#include <cutils/properties.h>
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <dlfcn.h>
#include <utils/Trace.h>
//#include <binder/IInterface.h>

namespace android
{

/* It should be sync with IPerfService.aidl */
enum {
    TRANSACTION_boostEnable = IBinder::FIRST_CALL_TRANSACTION,
    TRANSACTION_boostDisable,
    TRANSACTION_boostEnableTimeout,
    TRANSACTION_boostEnableTimeoutMs,
    TRANSACTION_notifyAppState,
    TRANSACTION_userReg,
    TRANSACTION_userRegBigLittle,
    TRANSACTION_userUnreg,
    TRANSACTION_userGetCapability,
    TRANSACTION_userRegScn,
    TRANSACTION_userRegScnConfig,
    TRANSACTION_userUnregScn,
    TRANSACTION_userEnable,
    TRANSACTION_userEnableTimeout,
    TRANSACTION_userEnableTimeoutMs,
    TRANSACTION_userEnableAsync,
    TRANSACTION_userEnableTimeoutAsync,
    TRANSACTION_userEnableTimeoutMsAsync,
    TRANSACTION_userDisable,
    TRANSACTION_userResetAll,
    TRANSACTION_userDisableAll,
    TRANSACTION_userRestoreAll,
    TRANSACTION_dumpAll,
    TRANSACTION_setFavorPid,
    TRANSACTION_restorePolicy,
    //TRANSACTION_getPackName,
    TRANSACTION_getLastBoostPid,
    TRANSACTION_notifyFrameUpdate,
    TRANSACTION_notifyDisplayType,
    TRANSACTION_notifyUserStatus,
    TRANSACTION_getClusterInfo,
    TRANSACTION_levelBoost
};

static int inited = 0;
static char packName[128] = "";
static sp<IServiceManager> sm ;

#define BOOT_INFO_FILE "/sys/class/BOOT/BOOT/boot/boot_mode"
#define RENDER_THREAD_UPDATE_DURATION   250000000
#define RENDER_THREAD_CHECK_DURATION    200000000

#define RENDER_BIT    0x800000
#define RENDER_MASK   0x7FFFFF

int (*perfCalcBoostLevel)(float) = NULL;
typedef int (*calc_boost_level)(float);

#define LEVEL_BOOST_NOP 0xffff

static int check_meta_mode(void)
{
    char bootMode[4];
    int fd;
    //check if in Meta mode
    fd = open(BOOT_INFO_FILE, O_RDONLY);
    if(fd < 0) {
        return 0; // not meta mode
    }

    if(read(fd, bootMode, 4) < 1) {
        close(fd);
        return 0;
    }

    if (bootMode[0] == 0x31) {
        close(fd);
        return 1; // meta mode
    }

    close(fd);
    return 0;
}

static void init(void)
{

}

extern "C"
void PerfServiceNative_boostEnable(int scenario)
{

}

extern "C"
void PerfServiceNative_boostDisable(int scenario)
{

}

extern "C"
void PerfServiceNative_boostEnableTimeout(int scenario, int timeout)
{

}

extern "C"
void PerfServiceNative_boostEnableTimeoutMs(int scenario, int timeout_ms)
{

}

extern "C"
void PerfServiceNative_boostEnableAsync(int scenario)
{
 
}

extern "C"
void PerfServiceNative_boostDisableAsync(int scenario)
{
  
}

extern "C"
void PerfServiceNative_boostEnableTimeoutAsync(int scenario, int timeout)
{
  
}

extern "C"
void PerfServiceNative_boostEnableTimeoutMsAsync(int scenario, int timeout_ms)
{
  
}

extern "C"
int PerfServiceNative_userReg(int scn_core, int scn_freq)
{
    int    handle = -1, pid=-1, tid=-1;
  
    return handle;
}

extern "C"
int PerfServiceNative_userRegBigLittle(int scn_core_big, int scn_freq_big, int scn_core_little, int scn_freq_little)
{
    int    handle = -1, pid=-1, tid=-1;

    return handle;
}

extern "C"
void PerfServiceNative_userUnreg(int handle)
{

}

extern "C"
int PerfServiceNative_userGetCapability(int cmd)
{

    int value = -1;
   
    return value;
}

extern "C"
int PerfServiceNative_userRegScn(void)
{
  
    int    handle = -1, pid=-1, tid=-1;
 
    return handle;
}

extern "C"
void PerfServiceNative_userRegScnConfig(int handle, int cmd, int param_1, int param_2, int param_3, int param_4)
{

}

extern "C"
void PerfServiceNative_userUnregScn(int handle)
{

}

extern "C"
void PerfServiceNative_userEnable(int handle)
{
}

extern "C"
void PerfServiceNative_userEnableTimeout(int handle, int timeout)
{
}

extern "C"
void PerfServiceNative_userEnableTimeoutMs(int handle, int timeout_ms)
{

}

extern "C"
void PerfServiceNative_userEnableAsync(int handle)
{

}

extern "C"
void PerfServiceNative_userEnableTimeoutAsync(int handle, int timeout)
{
 
}

extern "C"
void PerfServiceNative_userEnableTimeoutMsAsync(int handle, int timeout_ms)
{

}

extern "C"
void PerfServiceNative_userDisable(int handle)
{

}

extern "C"
void PerfServiceNative_userResetAll(void)
{

}

extern "C"
void PerfServiceNative_userDisableAll(void)
{

}

extern "C"
void PerfServiceNative_dumpAll(void)
{

}

extern "C"
void PerfServiceNative_setFavorPid(int pid)
{

}

extern "C"
void PerfServiceNative_setBoostThread(void)
{

}

extern "C"
void PerfServiceNative_restoreBoostThread(void)
{

}

extern "C"
void PerfServiceNative_notifyFrameUpdate(int level)
{
 
}

extern "C"
void PerfServiceNative_notifyRenderTime(float time)
{
 
}

extern "C"
void PerfServiceNative_notifyDisplayType(int type)
{

}

extern "C"
void PerfServiceNative_notifyUserStatus(int type, int status)
{

}

extern "C"
int PerfServiceNative_getLastBoostPid()
{

    return -1;
}

extern "C"
const char* PerfServiceNative_getPackName()
{
        return "";

}

extern "C"
int PerfServiceNative_getClusterInfo(int cmd, int id)
{
    Parcel data, reply;
    int value = -1;

    return value;
}

extern "C"
void PerfServiceNative_levelBoost(int level)
{

}

extern "C"
void PerfServiceNative_runtimeBoostEnable(void)
{
    PerfServiceNative_boostEnable(SCN_RUNTIME_BOOST);
}

extern "C"
void PerfServiceNative_runtimeBoostEnableTimeout(int timeout)
{
    PerfServiceNative_boostEnableTimeout(SCN_RUNTIME_BOOST, timeout);
}

extern "C"
void PerfServiceNative_runtimeBoostDisable(void)
{
    PerfServiceNative_boostDisable(SCN_RUNTIME_BOOST);
}


}
