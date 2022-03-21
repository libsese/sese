#include "CpuInfo.h"

#ifdef _WIN32
#pragma warning (disable : 4996)
#include <intrin.h>
#define cpuid __cpuid
#define cpuidex __cpuidex
#endif
#ifdef __linux__
#include <cpuid.h>
#include <cstring>
inline void cpuid(int cpuInfo[4], int function_id) {
    __cpuid(function_id, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
}

inline void cpuidex(int cpuInfo[4], int function_id, int subfunction_id) {
    __cpuid_count(function_id, subfunction_id, cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3]);
}
#endif

namespace sese {

    CpuInfo::CpuInfo() {
        Register reg{0, 0, 0, 0};
        // eax 为最高有效函数的 ID 编号
        cpuid((int *) &reg, 0);
        int ids = reg.eax;

        for (int i = 0; i <= ids; i++) {
            cpuidex((int *) &reg, i, 0);
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

        cpuid((int *) &reg, 0x80000000);
        int exIds = reg.eax;
        char tempBrand[64];
        for (int i = 0x80000000; i < exIds; i++) {
            cpuidex((int *) &reg, i, 0);
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

        // 解析 CPU 序列号
        char tempSerialNumber[17];
        sprintf(tempSerialNumber, "%08X%08X", data[1].edx, data[1].eax);
        serialNumber = tempSerialNumber;
    }

    uint64_t CpuInfo::RDTSC() noexcept {
#ifdef __linux__
        unsigned int lo, hi;
        __asm__ volatile("rdtsc"
                         : "=a"(lo), "=d"(hi));
        return ((uint64_t) hi << 32) | lo;
#endif
#ifdef _WIN32
        return __rdtsc();
#endif
    }

}// namespace sese