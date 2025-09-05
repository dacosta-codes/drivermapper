#include "includes.h"

typedef NTSTATUS(*RtlAdjustPrivilege_t)(
    ULONG,
    BOOLEAN,
    BOOLEAN,
    PBOOLEAN
    );

bool isIntelDriverRunning();
bool AcquireDebugPriv();
