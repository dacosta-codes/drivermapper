#include "intelDriverLoader.hpp"

bool isIntelDriverRunning() {
	HANDLE intelDriverHandle = CreateFileA("\\\\.\\Nal", FILE_ANY_ACCESS, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (intelDriverHandle != nullptr && intelDriverHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(intelDriverHandle);
		return true;
	}

	return false;
}

bool AcquireDebugPriv() {
    HMODULE hModule = GetModuleHandleA("ntdll.dll");
    if (!hModule) {
        return false;
    }

    RtlAdjustPrivilege_t RtlAdjustPrivilege = (RtlAdjustPrivilege_t)GetProcAddress(hModule, "RtlAdjustPrivilege");
    if (!RtlAdjustPrivilege) {
        return false;
    }

    BOOLEAN wasEnabled;
    NTSTATUS status = RtlAdjustPrivilege(20UL, TRUE, FALSE, &wasEnabled);
    if (!NT_SUCCESS(status)) {
        return false;
    }

    return true;
}