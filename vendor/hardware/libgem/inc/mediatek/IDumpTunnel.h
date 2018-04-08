#pragma GCC system_header
#ifndef ANDROID_GUI_IDUMPTUNNEL_H
#define ANDROID_GUI_IDUMPTUNNEL_H

#include <binder/IInterface.h>
#include <utils/Singleton.h>

namespace android
{

class IDumpTunnel : public IInterface {
protected:
    enum {
        DUMPTUNNEL_DUMP = IBinder::FIRST_CALL_TRANSACTION
    };

public:
    DECLARE_META_INTERFACE(DumpTunnel);

    virtual status_t kickDump(String8& /*result*/, const char* /*prefix*/) = 0;
};

class BnDumpTunnel : public BnInterface<IDumpTunnel>
{
    virtual status_t onTransact(uint32_t code,
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags = 0);
};

};
#endif
