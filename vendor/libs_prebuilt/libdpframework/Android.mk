LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE = libdpframework
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib64
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_64 = libc++ libion libm4u libsync libion_mtk libbinder # libpqservice libpq_prot libvcodecdrv 
LOCAL_EXPORT_C_INCLUDE_DIRS = $(LOCAL_PATH)/include
LOCAL_MULTILIB = 64
LOCAL_SRC_FILES_64 = libdpframework.so
include $(BUILD_PREBUILT)

include $(CLEAR_VARS)
LOCAL_MODULE = libdpframework
LOCAL_MODULE_CLASS = SHARED_LIBRARIES
LOCAL_MODULE_PATH := $(TARGET_OUT_VENDOR)/lib
LOCAL_MODULE_SUFFIX = .so
LOCAL_SHARED_LIBRARIES_32 = libc++ libion libm4u libsync libion_mtk libbinder # libpqservice libpq_prot libvcodecdrv 
LOCAL_EXPORT_C_INCLUDE_DIRS = $(LOCAL_PATH)/include
LOCAL_MULTILIB = 32
LOCAL_SRC_FILES_32 = arm/libdpframework.so
include $(BUILD_PREBUILT)
