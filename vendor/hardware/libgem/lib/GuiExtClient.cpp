#define LOG_TAG "GuiExt"

#define MTK_LOG_ENABLE 1
#include <cutils/log.h>
#include <binder/IServiceManager.h>
#include <binder/ProcessState.h>
#include "GuiExtClient.h"
#include "IGuiExtService.h"

namespace android {

#define GUIEX_LOGV(x, ...) ALOGV("[GuiExtL] " x, ##__VA_ARGS__)
#define GUIEX_LOGD(x, ...) ALOGD("[GuiExtL] " x, ##__VA_ARGS__)
#define GUIEX_LOGI(x, ...) ALOGI("[GuiExtL] " x, ##__VA_ARGS__)
#define GUIEX_LOGW(x, ...) ALOGW("[GuiExtL] " x, ##__VA_ARGS__)
#define GUIEX_LOGE(x, ...) ALOGE("[GuiExtL] " x, ##__VA_ARGS__)

GuiExtClient::GuiExtClient()
{
    assertStateLocked();
}

status_t GuiExtClient::assertStateLocked() const
{
    return NO_INIT;
}

void GuiExtClient::serviceDied()
{
    Mutex::Autolock _l(mLock);
    GUIEX_LOGI("[%s]", __func__);

    serviceDiedLocked();

    mGuiExtService.clear();
}
};
