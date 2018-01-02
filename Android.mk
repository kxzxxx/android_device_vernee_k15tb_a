LOCAL_PATH := $(call my-dir)

ifeq ($(TARGET_DEVICE),k15tb_a)

include $(call all-makefiles-under,$(LOCAL_PATH))

endif
