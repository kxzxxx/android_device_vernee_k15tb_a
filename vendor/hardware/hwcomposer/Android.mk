LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	hwc.cpp

LOCAL_CFLAGS := \
	-DLOG_TAG=\"hwcomposer\"

LOCAL_CFLAGS += -DMTK_VIRTUAL_SUPPORT

LOCAL_CFLAGS += -DMTK_ENHAHCE_SUPPORT

ifeq ($(TARGET_FORCE_HWC_FOR_VIRTUAL_DISPLAYS), true)
LOCAL_CFLAGS += -DMTK_FORCE_HWC_COPY_VDS
endif

ifeq ($(TARGET_RUNNING_WITHOUT_SYNC_FRAMEWORK), true)
LOCAL_CFLAGS += -DMTK_WITHOUT_PRIMARY_PRESENT_FENCE
endif

ifneq ($(filter kernel-3.18,$(LINUX_KERNEL_VERSION)),)
LOCAL_CFLAGS += -DMTK_CONTROL_POWER_WITH_FRAMEBUFFER_DEVICE
endif

MTK_HWC_CHIP = $(shell echo $(MTK_PLATFORM) | tr A-Z a-z )

# 1.5.0
MTK_HWC_VERSION := 1.5.0
LOCAL_CFLAGS += -DMTK_HWC_VER_1_5

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/include \
	system/core/include \
	$(LOCAL_PATH)/../libgem/inc \
	$(LOCAL_PATH)/../include

LOCAL_STATIC_LIBRARIES += \
	hwcomposer.$(TARGET_BOARD_PLATFORM).$(MTK_HWC_VERSION)

LOCAL_SHARED_LIBRARIES := \
    libui \
    liblog \
    libxlog \
	libgem \
	libutils \
	libcutils \
	libsync \
	libm4u \
	libion \
	libbwc \
	libion_mtk \
	libdpframework \
	libhardware \
	libgralloc_extra \
	libdl

ifeq ($(MTK_SEC_VIDEO_PATH_SUPPORT), yes)
LOCAL_CFLAGS += -DMTK_SVP_SUPPORT
endif # MTK_SEC_VIDEO_PATH_SUPPORT

ifneq ($(filter 1.4.0 1.4.0 1.4.0.sp 1.4.1 1.5.0,$(MTK_HWC_VERSION)),)
LOCAL_SHARED_LIBRARIES += \
	libged
ifneq ($(MTK_BASIC_PACKAGE), yes)
LOCAL_SHARED_LIBRARIES += \
	libperfservicenative
endif
endif

ifneq ($(MTK_BASIC_PACKAGE), yes)
LOCAL_SHARED_LIBRARIES += \
	libgas
endif # MTK_BASIC_PACKAGE

# HAL module implemenation stored in
# hw/<OVERLAY_HARDWARE_MODULE_ID>.<ro.product.board>.so
LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := hwcomposer.$(TARGET_BOARD_PLATFORM)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_RELATIVE_PATH := hw
include $(BUILD_SHARED_LIBRARY)

include $(call all-makefiles-under,$(LOCAL_PATH))



