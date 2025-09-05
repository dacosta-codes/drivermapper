#pragma once
#include "includes.h"

struct ImportFunctionInfo {
    std::string name;
    ULONG64* address;
};
struct ImportInfo {
    std::string moduleName;
    std::vector<ImportFunctionInfo> functionData;
};
struct RelocInfo {
    ULONG64 address;
    USHORT* item;
    ULONG32 count;
};