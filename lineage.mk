# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

DEVICE_PATH := device/vernee/k15tb_a
# Inherit from hardware-specific part of the product configuration
$(call inherit-product, $(DEVICE_PATH)/device.mk)

# Inherit some common Lineage stuff.
$(call inherit-product, vendor/lineage/config/common_full_phone.mk)

PRODUCT_NAME := lineage_k15tb_a
PRODUCT_DEVICE := k15tb_a
PRODUCT_BRAND := kx's
PRODUCT_MODEL := Apollo Lite
PRODUCT_MANUFACTURER := New-Bund
PRODUCT_RESTRICT_VENDOR_FILES := false
