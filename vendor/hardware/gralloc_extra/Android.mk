LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := \
	GraphicBufferExtra.cpp \
	GraphicBufferExtra_hal.cpp


LOCAL_C_INCLUDES := \
    frameworks/native/libs/nativewindow/include \
    frameworks/native/libs/nativebase/include \
    frameworks/native/libs/arect/include \
	$(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES := \
	liblog \
	libhardware \
	libcutils \
	libutils


LOCAL_PROPRIETARY_MODULE := true
LOCAL_MODULE := libgralloc_extra

include $(BUILD_SHARED_LIBRARY)
