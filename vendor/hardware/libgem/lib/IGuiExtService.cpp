#define LOG_TAG "GuiExt"

#define MTK_LOG_ENABLE 1
#include <utils/Errors.h>
#include <utils/RefBase.h>
#include <utils/Vector.h>
#include <utils/Timers.h>
#include <utils/String8.h>

#include <binder/Parcel.h>
#include <binder/IInterface.h>

#include <cutils/log.h>

#include <ui/GraphicBuffer.h>
#include <mediatek/IDumpTunnel.h>

#include "IGuiExtService.h"

namespace android {

// client : proxy GuiEx class
class BpGuiExtService : public BpInterface<IGuiExtService>
{
public:
    BpGuiExtService(const sp<IBinder>& impl) : BpInterface<IGuiExtService>(impl)
    {
    }

    virtual status_t alloc(const sp<IBinder>& token, uint32_t gralloc_usage, uint32_t w, uint32_t h, uint32_t *id)
    {
        return NO_ERROR;
    }

    virtual status_t free(uint32_t id)
    {
        return NO_ERROR;
    }

    virtual status_t acquire(const sp<IBinder>& token, uint32_t poolId, uint32_t usage, uint32_t type, int *buf)
    {
        return NO_ERROR;
    }

    virtual status_t request(uint32_t poolId, uint32_t usage, uint32_t type, int buf, sp<GraphicBuffer>* buffer)
    {

        return NO_ERROR;
    }

    virtual status_t release(uint32_t poolId, uint32_t usage, uint32_t type, int buf)
    {
        return NO_ERROR;
    }

    virtual status_t disconnect(uint32_t poolId, uint32_t usage, uint32_t type)
    {
        return NO_ERROR;
    }

    virtual status_t configDisplay(uint32_t type, bool enable, uint32_t w, uint32_t h, uint32_t bufNum)
    {
        return NO_ERROR;
    }
    virtual status_t regDump(const sp<IDumpTunnel>& tunnel, const String8& key)
    {
        return NO_ERROR;
    }

    virtual status_t unregDump(const String8& key)
    {
        return NO_ERROR;
    }
};

IMPLEMENT_META_INTERFACE(GuiExtService, "GuiExtService");

status_t BnGuiExtService::onTransact(uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    //ALOGD("receieve the command code %d", code);
        return NO_ERROR;
}

sp<IGuiExtService> checkGuiExtService()
{

    return NULL;
}
};

using namespace android;

bool regDump(const sp<IDumpTunnel>& tunnel, const String8& key)
{
return true;
}

bool unregDump(const String8& key)
{
return true;
}
