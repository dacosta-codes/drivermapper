#pragma once
#include <iostream>
#include <fstream>
#include <filesystem>
#include <Windows.h>
#include <vector>
#include <thread>
#include <chrono>
#include "structs.h"

bool ReadFileToMemory(std::string fileName, std::vector<BYTE>* outBuffer);
PIMAGE_NT_HEADERS64 getNtHeaders(void* image);
std::vector<ImportInfo> getImports(void* image);
std::vector<RelocInfo> getRelocations(void* image);