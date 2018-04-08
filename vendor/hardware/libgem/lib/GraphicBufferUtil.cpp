#define LOG_TAG "GraphicBufferUtil"
#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#define MTK_LOG_ENABLE 1
#include <utils/String8.h>

#include <cutils/log.h>

#include <ui/GraphicBufferExtra.h>
#include <ui/GraphicBuffer.h>
#include <ui/GraphicBufferMapper.h>

#include <png.h>

#include <GraphicBufferUtil.h>
#include <graphics_mtk_defs.h>

#define ALIGN_CEIL(x,a) (((x) + (a) - 1L) & ~((a) - 1L))
#define LOCK_FOR_SW (GRALLOC_USAGE_SW_READ_RARELY | GRALLOC_USAGE_SW_WRITE_RARELY | GRALLOC_USAGE_HW_TEXTURE)
#define SYMBOL2ALIGN(x) ((x) == 0) ? 1 : ((x) << 1)

namespace android {
// ---------------------------------------------------------------------------

ANDROID_SINGLETON_STATIC_INSTANCE( GraphicBufferUtil )

status_t BufferInfo::getInfo(const buffer_handle_t& handle)
{
    mHandle = handle;
    int format = PIXEL_FORMAT_UNKNOWN;
    int err = NO_ERROR;
    mErr = gralloc_extra_query(handle, GRALLOC_EXTRA_GET_WIDTH, &mWidth);
    mErr |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_HEIGHT, &mHeight);
    mErr |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_STRIDE, &mStride);
    mErr |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_VERTICAL_STRIDE, &mVStride);
    mErr |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_FORMAT, &mFormat);
    mErr |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_ALLOC_SIZE, &mSize);

    if ((err = getGraphicBufferUtil().getRealFormat(handle, &format)) == GRALLOC_EXTRA_OK)
    {
        mFormat = format;
    }
    mErr |= err;
    mErr |= updateVideoInfo(handle);

    return mErr;
}

status_t BufferInfo::getInfo(const sp<GraphicBuffer>& gb)
{
    if (NO_ERROR != getInfo(gb->handle))
    {
        ALOGD("Can't gralloc_extra handle, fallback to get info from gb!");
        mHandle = gb->handle;
        mWidth = gb->width;
        mHeight = gb->height;
        mStride = gb->stride;
        mFormat = gb->format;
        mErr = NO_ERROR;
    }
    return mErr;
}

status_t BufferInfo::updateVideoInfo(const buffer_handle_t& handle)
{
    int err = NO_ERROR;
    gralloc_extra_ion_sf_info_t info;

    err |= gralloc_extra_query(handle, GRALLOC_EXTRA_GET_IOCTL_ION_SF_INFO, &info);
    if (!err && info.videobuffer_status & 0x80000000)
    {
        unsigned int align = (info.videobuffer_status & 0x7FFFFFFF) >> 25;
        align = SYMBOL2ALIGN(align);
        mStride = ALIGN_CEIL(mWidth, align);

        align = (info.videobuffer_status & 0x0007FFFF) >> 13;
        align = SYMBOL2ALIGN(align);
        mVStride = ALIGN_CEIL(mHeight, align);
    }

    return err;
}

GraphicBufferUtil::GraphicBufferUtil()
{
}

GraphicBufferUtil::~GraphicBufferUtil()
{
}

static void write565Data(uint32_t width,
                         uint32_t height,
                         uint8_t* in,
                         png_structp out)
{

}

static void writeData(uint32_t width,
                      uint32_t height,
                      uint8_t* in,
                      png_structp out)
{
    // should be RGBA for 4 bytes

}

void GraphicBufferUtil::dump(const sp<GraphicBuffer>& gb,
                              const char* prefix,
                              const char* dir)
{
    if (CC_UNLIKELY(gb == NULL))
    {
        ALOGE("[%s] gb is NULL", __func__);
        return;
    }

    BufferInfo i;
    i.getInfo(gb);

    dump(i, prefix, dir);
}

void GraphicBufferUtil::dump(const buffer_handle_t &handle,
                              const char* prefix,
                              const char* dir)
{
    if (CC_UNLIKELY(handle == NULL))
    {
        ALOGE("[%s] handle is NULL", __func__);
        return;
    }

    BufferInfo i;
    i.getInfo(handle);

    dump(i, prefix, dir);
}

void GraphicBufferUtil::dump( const BufferInfo &info,
                              const char* prefix,
                              const char* dir)
{

}

uint32_t GraphicBufferUtil::getBitsPerPixel(int format)
{
    uint32_t bits = 0;

    return bits;
}

uint32_t GraphicBufferUtil::getBitsPerPixel(buffer_handle_t handle)
{
    int32_t format = 0;
    if (GraphicBufferExtra::get().query(
                handle,
                GRALLOC_EXTRA_GET_FORMAT,
                static_cast<int*>(&format)) != GRALLOC_EXTRA_OK)
    {
        ALOGE("Getting format of handle failed");
        return 0;
    }
    return getBitsPerPixel(format);
}

status_t GraphicBufferUtil::drawLine(const sp<GraphicBuffer>& gb, uint8_t val, int ptn_w, int ptn_h, int pos)
{
    if (CC_UNLIKELY(gb == NULL))
    {
        ALOGE("[%s] gb is NULL", __func__);
        return INVALID_OPERATION;
    }

    BufferInfo i;
    i.getInfo(gb);

    return drawLine(i, val, ptn_w, ptn_h, pos);
}

status_t GraphicBufferUtil::drawLine(const buffer_handle_t& handle, uint8_t val, int ptn_w, int ptn_h, int pos)
{
    if (CC_UNLIKELY(handle == NULL))
    {
        ALOGE("[%s] handle is NULL", __func__);
        return INVALID_OPERATION;
    }

    BufferInfo i;
    i.getInfo(handle);

    return drawLine(i, val, ptn_w, ptn_h, pos);
}

status_t GraphicBufferUtil::drawLine(const BufferInfo &info, uint8_t val, int ptn_w, int ptn_h, int pos)
{

    return NO_ERROR;
}

int GraphicBufferUtil::getRealFormat(buffer_handle_t handle, PixelFormat* format) {

    int err = GRALLOC_EXTRA_OK;

    return err;
}

// ---------------------------------------------------------------------------
}; // namespace android
