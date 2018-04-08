# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


#
# libgem.so
#


LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libgem

LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE_TAGS := optional


# === AOSP libui & libgui ==========================================================================
LOCAL_CLANG := true
LOCAL_CPPFLAGS := -std=c++1y -Weverything -Werror

# The static constructors and destructors in this library have not been noted to
# introduce significant overheads
LOCAL_CPPFLAGS += -Wno-exit-time-destructors
LOCAL_CPPFLAGS += -Wno-global-constructors

# We only care about compiling as C++14
LOCAL_CPPFLAGS += -Wno-c++98-compat-pedantic

# We use four-character constants for the GraphicBuffer header, and don't care
# that they're non-portable as long as they're consistent within one execution
LOCAL_CPPFLAGS += -Wno-four-char-constants

# We don't need to enumerate every case in a switch as long as a default case
# is present
LOCAL_CPPFLAGS += -Wno-switch-enum

# Allow calling variadic macros without a __VA_ARGS__ list
LOCAL_CPPFLAGS += -Wno-gnu-zero-variadic-macro-arguments

# Don't warn about struct padding
LOCAL_CPPFLAGS += -Wno-padded
# ==================================================================================================


# for bring up, please unmark this line
LOCAL_CFLAGS += -DMTK_DO_NOT_USE_GUI_EXT

ifneq ($(strip $(TARGET_BUILD_VARIANT)), eng)
LOCAL_CFLAGS += -DMTK_USER_BUILD
endif

ifeq ($(MTK_MIRAVISION_SUPPORT),yes)
LOCAL_CFLAGS += -DCONFIG_FOR_SOURCE_PQ
endif

LOCAL_SRC_FILES := \
	lib/IGuiExtService.cpp \
	lib/GuiExtService.cpp \
	lib/GuiExtClient.cpp \
	lib/GuiExtClientProducer.cpp \
	lib/GuiExtClientConsumer.cpp \
	lib/GuiExtImpl.cpp \
	lib/FpsCounter.cpp \
	lib/GraphicBufferUtil.cpp 

MTK_HWC_CHIP = $(shell echo $(MTK_PLATFORM) | tr A-Z a-z )


# FIXME: There are several warning issue in GuiExt*.cpp
# ignore warnings for GuiExt
LOCAL_CPPFLAGS += \
    -Wno-unused-parameter \
    -Wno-double-promotion \
	-Wno-sign-conversion \
	-Wno-gnu-statement-expression \
	-Wno-unreachable-code-break \
	-Wno-weak-vtables \
	-Wno-shadow \
	-Wno-unused-macros \
	-Wno-shorten-64-to-32 \
	-Wno-old-style-cast \
	-Wno-shift-sign-overflow \
	-Wno-missing-variable-declarations \
	-Wno-zero-length-array \
	-Wno-conversion \
	-Wno-undef \
	-Wno-float-equal \
	-Wno-documentation \
	-Wno-cast-align \
	-Wno-format

LOCAL_C_INCLUDES := \
	$(LOCAL_PATH)/inc \
	$(LOCAL_PATH)/../gralloc_extra/include \
    frameworks/native/libs/nativewindow/include \
    frameworks/native/libs/nativebase/include \
	system/core/include \
    frameworks/native/libs/arect/include \
	external/libpng \
	external/zlib \
	external/skia/src/images \
	external/skia/include/core

LOCAL_SHARED_LIBRARIES := \
	libutils \
	libcutils \
	libbinder \
	libhardware \
	libhardware_legacy \
	libdl \
	libion \
	libion_mtk \
	libgralloc_extra \
	libpng



# === AOSP libui ===================================================================================

LOCAL_SHARED_LIBRARIES += \
	libcutils \
	libhardware \
	libsync \
	libutils \
	liblog

ifneq ($(BOARD_FRAMEBUFFER_FORCE_FORMAT),)
LOCAL_CFLAGS += -DFRAMEBUFFER_FORCE_FORMAT=$(BOARD_FRAMEBUFFER_FORCE_FORMAT)
endif
# ==================================================================================================



# === AOSP libgui ==================================================================================


LOCAL_SHARED_LIBRARIES += \
	libbinder \
	libcutils \
	libEGL \
	libGLESv2 \
	libsync \
	libutils \
	liblog



# ==================================================================================================

include $(BUILD_SHARED_LIBRARY)

ifneq (,$(ONE_SHOT_MAKEFILE))
# If we're building with ( mm, mmm ), then also build guiext-*, and mat_test, Region_test, etc.
include $(call first-makefiles-under,$(LOCAL_PATH))
else
# otherwise, build guiext-* only. Don't overwrite libui and libgui nativetests.
include $(call all-makefiles-under,$(LOCAL_PATH))
endif
