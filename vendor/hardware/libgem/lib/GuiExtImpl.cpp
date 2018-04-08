#define LOG_TAG "GuiExt"
#define ATRACE_TAG ATRACE_TAG_GRAPHICS

#define MTK_LOG_ENABLE 1
#include <dlfcn.h>
#include <stdlib.h>
#include <utils/Timers.h>
#include <utils/Trace.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <math.h>

#include <ui/DisplayInfo.h>
#include <ui/GraphicBuffer.h>
#include <gui/Surface.h>
#include <gui/IProducerListener.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/CpuConsumer.h>
#include <hardware/gralloc.h>
#include <hardware/hwcomposer_defs.h>
#include <binder/IInterface.h>
#include <binder/IPCThreadState.h>

#include <hardware/gralloc_extra.h> 

#ifdef TOUCH_ION_BUFFER
#include <linux/ion_drv.h>
#include <ion/ion.h>
#endif

#include "GuiExtImpl.h"
#include "GuiExtDataType.h"

#include "graphics_mtk_defs.h"
#include "gralloc_mtk_defs.h"

#ifdef CONFIG_FOR_SOURCE_PQ
#include <sys/ioctl.h>
#include <linux/disp_session.h>
#endif

namespace android {

#define GUIEXT_LOGV(x, ...) ALOGV("[GuiExtI] " x, ##__VA_ARGS__)
#define GUIEXT_LOGD(x, ...) ALOGD("[GuiExtI] " x, ##__VA_ARGS__)
#define GUIEXT_LOGI(x, ...) ALOGI("[GuiExtI] " x, ##__VA_ARGS__)
#define GUIEXT_LOGW(x, ...) ALOGW("[GuiExtI] " x, ##__VA_ARGS__)
#define GUIEXT_LOGE(x, ...) ALOGE("[GuiExtI] " x, ##__VA_ARGS__)

#define ION_DEV_NODE "/dev/ion"

#define MAX_ALLOC_SIZE          3
#define MAX_GLES_DEQUEUED_NUM   3
#define MAX_HWC_DEQUEUED_NUM    3
#define LOCK_FOR_USAGE          (GRALLOC_USAGE_SW_READ_RARELY | GRALLOC_USAGE_SW_WRITE_NEVER | GRALLOC_USAGE_HW_TEXTURE)
#define GUIEXT_ION_CLIENT_ID    0

#define POOL_USAGE_SHIFT(usage)     (usage << 6)
#define POOL_TYPE_SHIFT(type)     (type << 3)
#define POOL_COMBINED_ID(usage, type, idx)   (POOL_USAGE_SHIFT(usage)|POOL_TYPE_SHIFT(type)|idx)

#define ALIGN_FLOOR(x,a)    ((x) & ~((a) - 1L))
#define ALIGN_CEIL(x,a)     (((x) + (a) - 1L) & ~((a) - 1L))

const char* szUsageName[2] = {"GuiExt_Gpu", "GuiExt_Hwc"};
const char* szDisplayType[HWC_NUM_DISPLAY_TYPES] = {"Primary", "External", "Virtual"};

uint32_t gAcquiredFormat[GUI_EXT_USAGE_MAX] = {HAL_PIXEL_FORMAT_YV12, HAL_PIXEL_FORMAT_YUYV};

#ifdef CONFIG_FOR_SOURCE_PQ
class DispDevice: public Singleton<DispDevice>
{
    int mDevFd;
    int mVideoRefCount;
    disp_caps_info mCaps;

public:
    DispDevice();
    ~DispDevice();

    status_t setOverlaySessionMode(DISP_MODE mode);
    uint32_t getPqNum() const {
        if (mCaps.max_pq_num == 0) {
            GUIEXT_LOGE("[PQ] Failed to get num of PQ");
        }

        return mCaps.max_pq_num;
    }
};

ANDROID_SINGLETON_STATIC_INSTANCE(DispDevice);

DispDevice::DispDevice()
    : mDevFd(-1)
    , mVideoRefCount(0)
{
    memset(&mCaps, 0, sizeof(disp_caps_info));

    char filename[256];
    mDevFd = open(filename, O_RDONLY);
    sprintf(filename, "/dev/%s", DISP_SESSION_DEVICE);
    if (mDevFd < 0) {
        GUIEXT_LOGE("[PQ] Failed to open display device(%s): fd=%d", filename, mDevFd);
    } else {
        GUIEXT_LOGD("[PQ] open display device(%s) successfully", filename);
        int err = ioctl(mDevFd, DISP_IOCTL_GET_DISPLAY_CAPS, &mCaps);
        if (err < 0) {
            GUIEXT_LOGE("[PQ] Failed to get disp capability err=%d", err);
        }
    }
}

DispDevice::~DispDevice()
{
    if (mDevFd >= 0)
        close(mDevFd);
}

status_t DispDevice::setOverlaySessionMode(DISP_MODE mode)
{
    const bool decouple = (mode == DISP_SESSION_DECOUPLE_MODE);
    GUIEXT_LOGD("[PQ] decouple=%d, VideoRefCnt=%d \n", decouple, mVideoRefCount);

    mVideoRefCount = decouple ? mVideoRefCount + 1: mVideoRefCount - 1;
    // If not the first VP start or last VP end, do nothing
    if ( !((mVideoRefCount == 1 && decouple) || (mVideoRefCount == 0 && !decouple)) ) {
        return NO_ERROR;
    }

    GUIEXT_LOGD("[PQ] Dynamic Switch to (mode=%d)", mode);

    disp_session_config config;
    memset(&config, 0, sizeof(disp_session_config));
    config.device_id  = HWC_DISPLAY_PRIMARY;
    config.type = DISP_SESSION_PRIMARY;
    config.session_id = MAKE_DISP_SESSION(config.type, config.device_id);
    config.mode       = mode;
    config.user       = SESSION_USER_GUIEXT;

    int err = ioctl(mDevFd, DISP_IOCTL_SET_SESSION_MODE, &config);
    if (err < 0)
    {
        GUIEXT_LOGE("[PQ] Failed to set DispSession (mode=%d, errno:%d)!!", mode, err);
        return BAD_VALUE;
    }
    return NO_ERROR;
}
#endif // CONFIG_FOR_SOURCE_PQ

// --------------------------------------------------------

GuiExtPool::GuiExtPool()
    : mPoolId(0)
    , mPoolList(NULL)
    , mDefaultDisplayWidth(0)
    , mDefaultDisplayHeight(0)
{
    GUIEXT_LOGI("GuiExtPool ctor");
}

GuiExtPool::~GuiExtPool()
{
}

status_t GuiExtPool::alloc(const sp<IBinder>& token, uint32_t gralloc_usage, uint32_t w, uint32_t h, uint32_t * poolId)
{

    return NO_ERROR;
}

status_t GuiExtPool::free(uint32_t poolId)
{
    return NO_ERROR;
}

status_t GuiExtPool::acquire(const sp<IBinder>& token, uint32_t poolId, uint32_t usage, uint32_t type, int *buf)
{

    return NO_ERROR;
}

status_t GuiExtPool::request(uint32_t poolId, uint32_t usage, uint32_t type, int buf, sp<GraphicBuffer>* buffer)
{

    return NO_ERROR;
}

status_t GuiExtPool::release(uint32_t poolId, uint32_t usage, uint32_t type, int buf)
{

    return NO_ERROR;
}

status_t GuiExtPool::disconnect(uint32_t poolId, uint32_t usage, uint32_t type)
{

    return NO_ERROR;
}

void GuiExtPool::configDisplay(uint32_t type, bool enable, uint32_t w, uint32_t h, uint32_t bufNum)
{

}

void GuiExtPool::removePoolItemLocked(uint32_t poolId)
{
  
}

void GuiExtPool::removePoolItem(uint32_t poolId)
{

}

void GuiExtPool::dump(String8& result) const
{
}

uint32_t GuiExtPool::getPoolId()
{
    mPoolId = (mPoolId+1)%MAX_ALLOC_SIZE;
    uint32_t id = mPoolId == 0 ? MAX_ALLOC_SIZE : mPoolId;
    sp<GuiExtPoolItem> item = mPoolList.valueFor(id);
    while (item != NULL) {
        mPoolId = (mPoolId+1)%MAX_ALLOC_SIZE;
        id = mPoolId == 0 ? MAX_ALLOC_SIZE : mPoolId;
        item = mPoolList.valueFor(id);
    }

    mPoolId = id;

    return id;
}

//------------------------------------------------------------------------------

GuiExtPoolItem::GuiExtPoolItem(const sp<IBinder>& token,
                bool isHwcNeeded,
                uint32_t poolId,
                uint32_t /*w*/,
                uint32_t /*h*/,
                DefaultKeyedVector< uint32_t, sp<DispInfo> >& dispList,
                sp<IBinder::DeathRecipient> observer)
                : mConsumerDeathListener(NULL)
                //, mGPUUsedBq(NULL)
                , mGPUUsedProducer(NULL)
                , mGPUUsedConsumer(NULL)
#if !SUPPORT_MULTIBQ_FOR_HWC
                , mHwcUsedBq(NULL)
#endif
                , mId(poolId)
                , mIsHwcNeeded(isHwcNeeded)
                , mGpuUsedBufNum(MAX_GLES_DEQUEUED_NUM)
                , mProducerPid(-1)
                , mProducerToken(token)
                , mProducerDeathObserver(observer)

{
    GUIEXT_LOGV("GuiExtPoolItem ctor, poolId=%d, isHwcNeeded=%d, token=%p", poolId, isHwcNeeded, token.get());

    //String8 name(szUsageName[0]);
    //name.appendFormat("_%d", poolId);
    //mGPUUsedBq = createBufferQueue(w, h, mGpuUsedBufNum, name);
    //createBufferQueue(w, h, mGpuUsedBufNum, name, &mGPUUsedProducer, &mGPUUsedConsumer);

#ifdef CONFIG_FOR_SOURCE_PQ
    if (DispDevice::getInstance().getPqNum() == 1)
    {
        DispDevice::getInstance().setOverlaySessionMode(DISP_SESSION_DECOUPLE_MODE);
    }
#endif
    if (mIsHwcNeeded) {
        String8 name(szUsageName[1]);
        name.appendFormat("_%d", poolId);
#if SUPPORT_MULTIBQ_FOR_HWC
        uint32_t disp_size = dispList.size();
        for (uint32_t i = 0; i < disp_size; i++) {
            uint32_t type = dispList[i]->type;
            String8 extname(name);
            extname.appendFormat("_%d", type);
            sp<HwcBqSlot> slot = new HwcBqSlot();
            //slot->bq = createBufferQueue(dispList[i]->w, dispList[i]->h, dispList[i]->bufNum, extname);
            createBufferQueue(dispList[i]->w, dispList[i]->h, dispList[i]->bufNum, extname, &slot->mProducer, &slot->mConsumer);
            slot->type = type;
            slot->bufNum = dispList[i]->bufNum;
            mHwcUsedBqList.add(type, slot);
        }
#else
        mHwcUsedBq = createBufferQueue(dispList[0]->w, dispList[0]->h, dispList[i]->bufNum, name);
#endif
    }

    mProducerPid = (NULL != token->localBinder())
                 ? getpid()
                 : IPCThreadState::self()->getCallingPid();

    for (uint32_t i = 0; i < GUI_EXT_USAGE_MAX; i++)
        mIsDisconnected[i] = true;
}

GuiExtPoolItem::~GuiExtPoolItem()
{
    //mGPUUsedBq.clear();
    mGPUUsedProducer.clear();
    mGPUUsedConsumer.clear();
#if SUPPORT_MULTIBQ_FOR_HWC
    mHwcUsedBqList.clear();
#else
    mHwcUsedBq.clear();
#endif

#ifdef CONFIG_FOR_SOURCE_PQ
    if (DispDevice::getInstance().getPqNum() == 1)
    {
        DispDevice::getInstance().setOverlaySessionMode(DISP_SESSION_DIRECT_LINK_MODE);
    }
#endif
}

void GuiExtPoolItem::createBufferQueue(uint32_t w, uint32_t h, uint32_t num, String8 name,
    sp<IGraphicBufferProducer>* p, sp<IGraphicBufferConsumer>* c)
{

}

status_t GuiExtPoolItem::prepareBuffer(uint32_t gralloc_usage)
{

    return NO_ERROR;
}

status_t GuiExtPoolItem::prepareBuffer(sp<IGraphicBufferProducer> producer, uint32_t usage, uint32_t type, uint32_t gralloc_usage, uint32_t bufNum)
{


    return NO_ERROR;
}

#ifdef TOUCH_ION_BUFFER
buffer_handle_t GuiExtPoolItem::getNativehandle(android_native_buffer_t* buffer)
{
    if ((buffer->common.magic   == ANDROID_NATIVE_BUFFER_MAGIC) &&
        (buffer->common.version == sizeof(android_native_buffer_t)))
    {
        return buffer->handle;
    }
    return 0;
}

void GuiExtPoolItem::touchIonBuffer(sp<GraphicBuffer> gb)
{
    int ion_fd = -1;
    android_native_buffer_t* buffer = gb.get();
    buffer_handle_t hnd = getNativehandle(buffer);
    int err = 0;
    uint32_t bufSize = 0;
    GuiExtIONDevice &device(GuiExtIONDevice::getInstance());

    err |= gralloc_extra_query(hnd, GRALLOC_EXTRA_GET_ION_FD, &ion_fd);

    uint32_t ion_hnd = device.import(ion_fd);
    device.touchMMUAddress(0, ion_hnd);
    close(ion_hnd);
}
#endif

status_t GuiExtPoolItem::acquire(const sp<IBinder>& token, uint32_t usage, uint32_t type, sp<BinderDiedListener> listener, int *buf)
{


    return NO_ERROR;
}

status_t GuiExtPoolItem::request(uint32_t usage, uint32_t type, int buf, sp<GraphicBuffer>* buffer)
{

    return NO_ERROR;
}

status_t GuiExtPoolItem::release(uint32_t usage, uint32_t type, int buf)
{


    return NO_ERROR;
}

sp<IGraphicBufferProducer> GuiExtPoolItem::getIGraphicBufferProducer(uint32_t usage, uint32_t type)
{

    return NULL;
}

status_t GuiExtPoolItem::disconnect(uint32_t usage, uint32_t /*type*/)
{
    return NO_ERROR;
}

void GuiExtPoolItem::dump(String8& result) const
{
   
}

void GuiExtPoolItem::ConsumerSlot::dump(String8& result) const
{
    result.appendFormat("[%02d] pid=%d, token=%p, observer=%p\n", idx, pid, token.get(), observer.get());
}

// ---------------------------------------------------------------------------

#ifdef TOUCH_ION_BUFFER
ANDROID_SINGLETON_STATIC_INSTANCE(GuiExtIONDevice);

GuiExtIONDevice::GuiExtIONDevice()
{
    m_dev_fd = open(ION_DEV_NODE, O_RDONLY);
    if (m_dev_fd <= 0)
    {
        GUIEXT_LOGE("Failed to open ION device: %s ", strerror(errno));
    }
}

GuiExtIONDevice::~GuiExtIONDevice()
{
    if (m_dev_fd > 0) close(m_dev_fd);
}

uint32_t GuiExtIONDevice::import(int ion_fd)
{
    if (m_dev_fd <= 0) return 0;

    struct ion_handle* ion_hnd;
    if (ion_import(m_dev_fd, ion_fd, &ion_hnd))
    {
        GUIEXT_LOGE("Failed to import ION handle: %s ", strerror(errno));
        return 0;
    }

    return (unsigned int)ion_hnd;
}

void GuiExtIONDevice::touchMMUAddress(int client, int ion_hnd)
{
    if (m_dev_fd <= 0) return;

    // configure before querying physical address
    struct ion_mm_data mm_data;
    mm_data.mm_cmd                        = ION_MM_CONFIG_BUFFER;
    mm_data.config_buffer_param.handle    = (struct ion_handle *) ion_hnd;
    mm_data.config_buffer_param.eModuleID = client;
    mm_data.config_buffer_param.security  = 0;
    mm_data.config_buffer_param.coherent  = 0;

    int status = ion_custom_ioctl(m_dev_fd, ION_CMD_MULTIMEDIA, &mm_data);
    if ((status > 0) && (status != ION_ERROR_CONFIG_LOCKED))
    {
        GUIEXT_LOGE("Failed to config ION memory: %s", strerror(errno));
        return;
    }

    // get physical address
    struct ion_sys_data sys_data;
    sys_data.sys_cmd               = ION_SYS_GET_PHYS;
    sys_data.get_phys_param.handle = (struct ion_handle *) ion_hnd;
    if (ion_custom_ioctl(m_dev_fd, ION_CMD_SYSTEM, &sys_data))
    {
        GUIEXT_LOGE("Failed to get MVA from ION: %s", strerror(errno));
        return;
    }
}
#endif

}
