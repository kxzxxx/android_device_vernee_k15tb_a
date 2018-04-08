#define LOG_TAG "GuiExt"

#define MTK_LOG_ENABLE 1
#include <dlfcn.h>
#include <fcntl.h>
#include <math.h>
#include <string.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include <utils/SortedVector.h>
#include <binder/PermissionCache.h>

#include <android/native_window.h>
#include <gui/ISurfaceComposer.h>
#include <gui/SurfaceComposerClient.h>

#include <ui/DisplayInfo.h>
#include <mediatek/IDumpTunnel.h>

#include <cutils/memory.h>

#include "GuiExtService.h"
#include "GuiExtImpl.h"

namespace android {

#define GUIEXT_LOGV(x, ...) ALOGV("[GuiExtS] " x, ##__VA_ARGS__)
#define GUIEXT_LOGD(x, ...) ALOGD("[GuiExtS] " x, ##__VA_ARGS__)
#define GUIEXT_LOGI(x, ...) ALOGI("[GuiExtS] " x, ##__VA_ARGS__)
#define GUIEXT_LOGW(x, ...) ALOGW("[GuiExtS] " x, ##__VA_ARGS__)
#define GUIEXT_LOGE(x, ...) ALOGE("[GuiExtS] " x, ##__VA_ARGS__)

GuiExtService::GuiExtService()
{
    GUIEXT_LOGI("GuiExtService ctor");
    mPool = new GuiExtPool();
}

GuiExtService::~GuiExtService()
{
}

status_t GuiExtService::alloc(const sp<IBinder>& token, uint32_t gralloc_usage, uint32_t w, uint32_t h, uint32_t *poolId)
{
    GUIEXT_LOGV("alloc, gralloc_usage=%x, w=%d, h=%d", gralloc_usage, w, h);
    status_t ret = mPool->alloc(token, gralloc_usage, w, h, poolId);
    return ret;
}

status_t GuiExtService::free(uint32_t poolId)
{
    GUIEXT_LOGV("free, poolId=%d", poolId);
    status_t ret = mPool->free(poolId);
    return ret;
}

status_t GuiExtService::acquire(const sp<IBinder>& token, uint32_t poolId, uint32_t usage, uint32_t type, int *buf)
{
    GUIEXT_LOGV("acquire, poolId=%d, usage=%x, type=%d", poolId, usage, type);
    status_t ret = mPool->acquire(token, poolId, usage, type, buf);
    return ret;
}

status_t GuiExtService::request(uint32_t poolId, uint32_t usage, uint32_t type, int buf, sp<GraphicBuffer>* buffer)
{
    GUIEXT_LOGV("request, poolId=%d, usage=%x, type=%d, buf=%d", poolId, usage, type, buf);
    status_t ret = mPool->request(poolId, usage, type, buf, buffer);
    return ret;
}

status_t GuiExtService::release(uint32_t poolId, uint32_t usage, uint32_t type, int buf)
{
    GUIEXT_LOGV("release, poolId=%d, usage=%x, buf=%d, type=%d", poolId, usage, buf, type);
    status_t ret = mPool->release(poolId, usage, type, buf);
    return ret;
}

status_t GuiExtService::disconnect(uint32_t poolId, uint32_t usage, uint32_t type)
{
    GUIEXT_LOGV("disconnect, poolId=%d, usage=%x, type=%d", poolId, usage, type);
    status_t ret = mPool->disconnect(poolId, usage, type);
    return ret;
}

status_t GuiExtService::configDisplay(uint32_t type, bool enable, uint32_t w, uint32_t h, uint32_t bufNum)
{
    GUIEXT_LOGV("configDisplay, type=%d, enable=%d, w=%d, h=%d, bufNum=%d", type, enable, w, h, bufNum);
    mPool->configDisplay(type, enable, w, h, bufNum);
    return NO_ERROR;
}

static const String16 sDump("android.permission.DUMP");
status_t GuiExtService::dump(int fd, const Vector<String16>& /*args*/)
{

    return NO_ERROR;
}

status_t GuiExtService::regDump(const sp<IDumpTunnel>& tunnel, const String8& key)
{
    // check the tunnel does not come from GuiExtService
    if (!tunnel->asBinder(tunnel)->remoteBinder())
        return NO_ERROR;

    if (!tunnel->asBinder(tunnel)->isBinderAlive())
        return BAD_VALUE;

    class DeathNotifier : public IBinder::DeathRecipient
    {
    private:
        const String8 mKey;
        const wp<GuiExtService> mService;
    public:
        DeathNotifier(const String8& key, const wp<GuiExtService>& service)
            : mKey(key)
            , mService(service)
        {
            // since DeathNotifier will not be kept by linkToDeath
            // we have to extend its lifetime manually
            extendObjectLifetime(OBJECT_LIFETIME_WEAK);
        }

        virtual void binderDied(const wp<IBinder>& /*who*/)
        {
            GUIEXT_LOGD("binder of dump tunnel(%s) died", mKey.string());

            sp<GuiExtService> service = mService.promote();
            if (service != NULL)
                service->unregDump(mKey);
        }
    };
    sp<IBinder::DeathRecipient> notifier = new DeathNotifier(key, this);
    if (notifier != NULL)
        tunnel->asBinder(tunnel)->linkToDeath(notifier);

    Mutex::Autolock l(mDumpLock);

    if (mDumpTunnels.size() > 200)
    {
        const int32_t before = mDumpTunnels.size();

        // loop and remove zombie objects
        for (int32_t i = (before - 1); i >= 0; i--)
        {
            const sp<IDumpTunnel>& t = mDumpTunnels[i];
            if (!t->asBinder(t)->isBinderAlive())
            {
                mDumpTunnels.removeItemsAt(i);
            }
        }

        GUIEXT_LOGI("mDumpTunnels checked (before=%d, after:%zu", before, mDumpTunnels.size());
    }

    mDumpTunnels.add(key, tunnel);
    return NO_ERROR;
}

status_t GuiExtService::unregDump(const String8& key)
{
    Mutex::Autolock l(mDumpLock);
    mDumpTunnels.removeItem(key);
    return NO_ERROR;
}
};
