#include "system/CpuInfo.h"
#include <bitset>
#include <intrin.h>
#include <thread>
#include <vector>
#pragma warning(disable : 4996)

#define cpuid __cpuid
#define cpuidex __cpuidex

namespace sese {

    static bool isINTEL = false;
    static bool isAMD = false;
    static bool isARM = false;

    static std::string vendor;
    static std::string brand;
    static std::string serialNumber;
    static uint32_t cores = 0;

    bool CpuInfo::isIntel() noexcept { return isINTEL; }
    bool CpuInfo::isAmd() noexcept { return isAMD; }
    bool CpuInfo::isArm() noexcept { return isARM; }
    const std::string &CpuInfo::getVendor() noexcept { return vendor; }
    const std::string &CpuInfo::getBrand() noexcept { return brand; }
    const std::string &CpuInfo::getSerialNumber() noexcept { return serialNumber; }
    uint32_t CpuInfo::getLogicProcessors() noexcept { return std::thread::hardware_concurrency(); }

#ifdef SESE_ARCH_X64
    namespace _windows {

        /// CPUID 需要用上的 4 个寄存器
        struct Register {
            int32_t eax;
            int32_t ebx;
            int32_t ecx;
            int32_t edx;
        };

        static std::bitset<32> ECX1;
        static std::bitset<32> EDX1;
        static std::bitset<32> EBX7;
        static std::bitset<32> ECX7;
        static std::bitset<32> ECX81;
        static std::bitset<32> EDX81;
        static std::vector<Register> data;
        static std::vector<Register> exData;

        static struct CpuInitStruct {
            CpuInitStruct() {
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
        } cpuInitStruct; /* NOLINT */

    }// namespace _windows

    bool CpuInfo::SSE3() noexcept { return _windows::ECX1[0]; }
    bool CpuInfo::PCLMULQDQ() noexcept { return _windows::ECX1[1]; }
    bool CpuInfo::MONITOR() noexcept { return _windows::ECX1[3]; }
    bool CpuInfo::SSSE3() noexcept { return _windows::ECX1[9]; }
    bool CpuInfo::FMA() noexcept { return _windows::ECX1[12]; }
    bool CpuInfo::CMPXCHG16B() noexcept { return _windows::ECX1[13]; }
    bool CpuInfo::SSE41() noexcept { return _windows::ECX1[19]; }
    bool CpuInfo::SSE42() noexcept { return _windows::ECX1[20]; }
    bool CpuInfo::MOVBE() noexcept { return _windows::ECX1[22]; }
    bool CpuInfo::POPCNT() noexcept { return _windows::ECX1[23]; }
    bool CpuInfo::AES() noexcept { return _windows::ECX1[25]; }
    bool CpuInfo::XSAVE() noexcept { return _windows::ECX1[26]; }
    bool CpuInfo::OSXSAVE() noexcept { return _windows::ECX1[27]; }
    bool CpuInfo::AVX() noexcept { return _windows::ECX1[28]; }
    bool CpuInfo::F16C() noexcept { return _windows::ECX1[29]; }
    bool CpuInfo::RDRAND() noexcept { return _windows::ECX1[30]; }

    bool CpuInfo::MSR() noexcept { return _windows::EDX1[5]; }
    bool CpuInfo::CX8() noexcept { return _windows::EDX1[8]; }
    bool CpuInfo::SEP() noexcept { return _windows::EDX1[11]; }
    bool CpuInfo::CMOV() noexcept { return _windows::EDX1[15]; }
    bool CpuInfo::CLFSH() noexcept { return _windows::EDX1[19]; }
    bool CpuInfo::MMX() noexcept { return _windows::EDX1[23]; }
    bool CpuInfo::FXSR() noexcept { return _windows::EDX1[24]; }
    bool CpuInfo::SSE() noexcept { return _windows::EDX1[25]; }
    bool CpuInfo::SSE2() noexcept { return _windows::EDX1[26]; }

    bool CpuInfo::FSGSBASE() noexcept { return _windows::EBX7[0]; }
    bool CpuInfo::BMI1() noexcept { return _windows::EBX7[3]; }
    bool CpuInfo::HLE() noexcept { return isINTEL && _windows::EBX7[4]; }
    bool CpuInfo::AVX2() noexcept { return _windows::EBX7[5]; }
    bool CpuInfo::BMI2() noexcept { return _windows::EBX7[8]; }
    bool CpuInfo::ERMS() noexcept { return _windows::EBX7[9]; }
    bool CpuInfo::INVPCID() noexcept { return _windows::EBX7[10]; }
    bool CpuInfo::RTM() noexcept { return isINTEL && _windows::EBX7[11]; }
    bool CpuInfo::AVX512F() noexcept { return _windows::EBX7[16]; }
    bool CpuInfo::RDSEED() noexcept { return _windows::EBX7[18]; }
    bool CpuInfo::ADX() noexcept { return _windows::EBX7[19]; }
    bool CpuInfo::AVX512PF() noexcept { return _windows::EBX7[26]; }
    bool CpuInfo::AVX512ER() noexcept { return _windows::EBX7[27]; }
    bool CpuInfo::AVX512CD() noexcept { return _windows::EBX7[28]; }
    bool CpuInfo::SHA() noexcept { return _windows::EBX7[29]; }

    bool CpuInfo::PREFETCHWT1() noexcept { return _windows::ECX7[0]; }

    bool CpuInfo::LAHF() noexcept { return _windows::ECX81[0]; }
    bool CpuInfo::LZCNT() noexcept { return isINTEL && _windows::ECX81[5]; }
    bool CpuInfo::ABM() noexcept { return isAMD && _windows::ECX81[5]; }
    bool CpuInfo::SSE4a() noexcept { return isAMD && _windows::ECX81[6]; }
    bool CpuInfo::XOP() noexcept { return isAMD && _windows::ECX81[11]; }
    bool CpuInfo::TBM() noexcept { return isAMD && _windows::ECX81[21]; }

    bool CpuInfo::SYSCALL() noexcept { return isINTEL && _windows::EDX81[11]; }
    bool CpuInfo::MMXEXT() noexcept { return isAMD && _windows::EDX81[22]; }
    bool CpuInfo::RDTSCP() noexcept { return isINTEL && _windows::EDX81[27]; }
    bool CpuInfo::_3DNOWEXT() noexcept { return isAMD && _windows::EDX81[30]; }
    bool CpuInfo::_3DNOW() noexcept { return isAMD && _windows::EDX81[31]; }

    uint64_t CpuInfo::RDTSC() noexcept {
        return __rdtsc();
    }

#endif
    //todo 完善 Windows 下 Arm 架构的识别
#ifdef SESE_ARCH_ARM64
    namespace _windows {
        static struct CpuInitStruct {
            CpuInitStruct() {
                isARM = true;
            }
        } cpuInitStruct; /* NOLINT */
    } // namespace _windows
#endif
}// namespace sese
