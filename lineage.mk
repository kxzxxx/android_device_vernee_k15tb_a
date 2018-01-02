$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

# Inherit from the common Open Source product configuration
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# Inherit from hardware-specific part of the product configuration
$(call inherit-product, device/vernee/k15tb_a/device.mk)

# Inherit some common CM stuff.
$(call inherit-product, vendor/cm/config/common_full_phone.mk)


# Release name

PRODUCT_NAME := lineage_k15tb_a
PRODUCT_DEVICE := k15tb_a
PRODUCT_BRAND := Vernee
PRODUCT_MODEL := Apollo Lite
PRODUCT_MANUFACTURER := Vernee
PRODUCT_RESTRICT_VENDOR_FILES := false

# Boot animation
TARGET_SCREEN_HEIGHT      := 1920
TARGET_SCREEN_WIDTH       := 1080
TARGET_BOOTANIMATION_NAME := 1080

