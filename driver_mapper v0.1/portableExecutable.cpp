#include "portableExecutable.hpp"
#include "structs.h"
#include "includes.h"

bool ReadFileToMemory(std::string fileName, std::vector<BYTE>* outBuffer) {
    if (!std::filesystem::exists(fileName)) {
        return false;
    }

    std::ifstream file_image(fileName, std::ios::binary);
    outBuffer->assign(std::istreambuf_iterator<char>(file_image), std::istreambuf_iterator<char>());
    file_image.close();
    return true;
}
PIMAGE_NT_HEADERS64 getNtHeaders(void* image) {
    PIMAGE_DOS_HEADER dosHeader = PIMAGE_DOS_HEADER(image);
    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
        return nullptr;
    }

    PIMAGE_NT_HEADERS64 ntHeaders = (PIMAGE_NT_HEADERS64)((uintptr_t)image + (uintptr_t)dosHeader->e_lfanew);
    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE) {
        return nullptr;
    }

    return ntHeaders;
}
std::vector<ImportInfo> getImports(void* image) {
    PIMAGE_NT_HEADERS64 ntHeaders = getNtHeaders(image);
    if (!ntHeaders) return {};

    ULONG32 importRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    PIMAGE_IMPORT_DESCRIPTOR currentImport = (PIMAGE_IMPORT_DESCRIPTOR)((uintptr_t)image + (uintptr_t)importRVA);

    std::vector<ImportInfo> importInfo;
    while (currentImport->FirstThunk) {
        ImportInfo module;
        module.moduleName = std::string((char*)(uintptr_t)image + (uintptr_t)currentImport->Name);

        PIMAGE_THUNK_DATA64 firstThunk = (PIMAGE_THUNK_DATA64)((uintptr_t)image + (uintptr_t)currentImport->FirstThunk);
        PIMAGE_THUNK_DATA64 originalThunk = (PIMAGE_THUNK_DATA64)((uintptr_t)image + (uintptr_t)currentImport->OriginalFirstThunk);

        while (originalThunk->u1.Function) {
            ImportFunctionInfo importFunctionInfo;

            PIMAGE_IMPORT_BY_NAME thunkData = (PIMAGE_IMPORT_BY_NAME)((uintptr_t)image + (uintptr_t)originalThunk->u1.AddressOfData);

            importFunctionInfo.name = thunkData->Name;
            importFunctionInfo.address = &firstThunk->u1.Function;

            firstThunk++;
            originalThunk++;
        }

        importInfo.push_back(module);
        currentImport++;
    }
    return importInfo;
}
std::vector<RelocInfo> getRelocations(void* image) {
    PIMAGE_NT_HEADERS64 ntHeaders = getNtHeaders(image);
    if (!ntHeaders) return {};

    ULONG32 baseRelocationRVA = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    ULONG32 sizeOfRelocations = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
    PIMAGE_BASE_RELOCATION currentBaseRelocation = (PIMAGE_BASE_RELOCATION)((uintptr_t)image + (uintptr_t)baseRelocationRVA);
    PIMAGE_BASE_RELOCATION endOfBaseRelocation = (PIMAGE_BASE_RELOCATION)((uintptr_t)currentBaseRelocation + (uintptr_t)sizeOfRelocations);

    std::vector<RelocInfo> relocs;
    while (currentBaseRelocation < endOfBaseRelocation && currentBaseRelocation->SizeOfBlock) {
        RelocInfo relocInfo;
        relocInfo.address = (uintptr_t)image + (uintptr_t)currentBaseRelocation->VirtualAddress;
        relocInfo.item = (USHORT*)((uintptr_t)currentBaseRelocation + sizeof(IMAGE_BASE_RELOCATION));
        relocInfo.count = (currentBaseRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(USHORT);
        relocs.push_back(relocInfo);
        currentBaseRelocation = (PIMAGE_BASE_RELOCATION)((uintptr_t)(currentBaseRelocation + currentBaseRelocation->SizeOfBlock));
    }

    return relocs;
}
