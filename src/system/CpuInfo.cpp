#include "CpuInfo.h"

#ifdef _WIN32
#include <intrin.h>
#endif

namespace sese {

    CpuInfo::CpuInfo() {
#ifdef _WIN32
        Register reg{0, 0, 0, 0};
        // eax 为最高有效函数的 ID 编号
        __cpuid((int *) &reg, 0);
        int ids = reg.eax;

        for (int i = 0; i <= ids; i++) {
            __cpuidex((int *) &reg, i, 0);
            data.push_back(reg);
        }

        // 读取制造商字符串
        char tempVendor[32]{0};
        *reinterpret_cast<int *>(tempVendor + 0) = data[0].ebx;
        *reinterpret_cast<int *>(tempVendor + 4) = data[0].edx;
        *reinterpret_cast<int *>(tempVendor + 8) = data[0].ecx;
        vendor = tempVendor;
        if (vendor == VENDOR_INTEL_STRING) {
            isINTEL = true;
        } else if (vendor == VENDOR_AMD_STRING) {
            isAMD = true;
        }

        // 0x00000001 开始的一些标志
        if (ids >= 0x00000001) {
            ECX1 = data[1].ecx;
            EDX1 = data[1].edx;
        }
        // 0x00000007 开始的一些标志
        if (ids >= 0x00000007) {
            EBX7 = data[7].eax;
            ECX7 = data[7].ebx;
        }

        __cpuid((int *) &reg, 0x80000000);
        int exIds = reg.eax;
        char tempBrand[64];
        for (int i = 0x80000000; i < exIds; i++) {
            __cpuidex((int *) &reg, i, 0);
            exData.push_back(reg);
        }

        // 0x80000001 开始的一些标志
        if (exIds >= 0x80000001) {
            ECX81 = exData[1].ecx;
            EDX81 = exData[1].edx;
        }
        // 解析 CPU 具体型号
        if (exIds >= 0x80000004) {
            memcpy(tempBrand + 0, (int *) &exData[2], sizeof(Register));
            memcpy(tempBrand + 16, (int *) &exData[3], sizeof(Register));
            memcpy(tempBrand + 32, (int *) &exData[4], sizeof(Register));
            brand = tempBrand;
        }
#endif
    }

}// namespace sese