#include "includes.h"
#include "structs.h"
#include "portableExecutable.hpp"
#include "intelDriverLoader.hpp"

int main()
{
    if (isIntelDriverRunning()) {
        std::cout << "[-] vunerable driver already loaded" << std::endl;
    }

    if (!AcquireDebugPriv()) {
        std::cout << "[-] failed to elevate permissions" << std::endl;
    }

    system("pause");
    return 0;
}

