#pragma once
#include "Config.h"
#include <bitset>
#include <vector>

#define VENDOR_INTEL_STRING "GenuineIntel"
#define VENDOR_AMD_STRING "AuthenticAMD"

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    struct Register {
        int eax;
        int ebx;
        int ecx;
        int edx;
    };

    class API CpuInfo {
    public:
        CpuInfo();

    private:
        bool isINTEL;
        bool isAMD;
        std::string vendor;
        std::string brand;
        std::string serialNumber;

    public:
        [[nodiscard]] bool isIntel() const noexcept { return isINTEL; }
        [[nodiscard]] bool isAmd() const noexcept { return isAMD; }
        [[nodiscard]] const std::string &getVendor() const noexcept { return vendor; }
        [[nodiscard]] const std::string &getBrand() const noexcept { return brand; }
        [[nodiscard]] const std::string &getSerialNumber() const noexcept { return serialNumber; }

        [[nodiscard]] bool SSE3() const noexcept { return this->ECX1[0]; }
        [[nodiscard]] bool PCLMULQDQ() const noexcept { return this->ECX1[1]; }
        [[nodiscard]] bool MONITOR() const noexcept { return this->ECX1[3]; }
        [[nodiscard]] bool SSSE3() const noexcept { return this->ECX1[9]; }
        [[nodiscard]] bool FMA() const noexcept { return this->ECX1[12]; }
        [[nodiscard]] bool CMPXCHG16B() const noexcept { return this->ECX1[13]; }
        [[nodiscard]] bool SSE41() const noexcept { return this->ECX1[19]; }
        [[nodiscard]] bool SSE42() const noexcept { return this->ECX1[20]; }
        [[nodiscard]] bool MOVBE() const noexcept { return this->ECX1[22]; }
        [[nodiscard]] bool POPCNT() const noexcept { return this->ECX1[23]; }
        [[nodiscard]] bool AES() const noexcept { return this->ECX1[25]; }
        [[nodiscard]] bool XSAVE() const noexcept { return this->ECX1[26]; }
        [[nodiscard]] bool OSXSAVE() const noexcept { return this->ECX1[27]; }
        [[nodiscard]] bool AVX() const noexcept { return this->ECX1[28]; }
        [[nodiscard]] bool F16C() const noexcept { return this->ECX1[29]; }
        [[nodiscard]] bool RDRAND() const noexcept { return this->ECX1[30]; }

        [[nodiscard]] bool MSR() const noexcept { return this->EDX1[5]; }
        [[nodiscard]] bool CX8() const noexcept { return this->EDX1[8]; }
        [[nodiscard]] bool SEP() const noexcept { return this->EDX1[11]; }
        [[nodiscard]] bool CMOV() const noexcept { return this->EDX1[15]; }
        [[nodiscard]] bool CLFSH() const noexcept { return this->EDX1[19]; }
        [[nodiscard]] bool MMX() const noexcept { return this->EDX1[23]; }
        [[nodiscard]] bool FXSR() const noexcept { return this->EDX1[24]; }
        [[nodiscard]] bool SSE() const noexcept { return this->EDX1[25]; }
        [[nodiscard]] bool SSE2() const noexcept { return this->EDX1[26]; }

        [[nodiscard]] bool FSGSBASE() const noexcept { return this->EBX7[0]; }
        [[nodiscard]] bool BMI1() const noexcept { return this->EBX7[3]; }
        [[nodiscard]] bool HLE() const noexcept { return this->isINTEL && this->EBX7[4]; }
        [[nodiscard]] bool AVX2() const noexcept { return this->EBX7[5]; }
        [[nodiscard]] bool BMI2() const noexcept { return this->EBX7[8]; }
        [[nodiscard]] bool ERMS() const noexcept { return this->EBX7[9]; }
        [[nodiscard]] bool INVPCID() const noexcept { return this->EBX7[10]; }
        [[nodiscard]] bool RTM() const noexcept { return this->isINTEL && this->EBX7[11]; }
        [[nodiscard]] bool AVX512F() const noexcept { return this->EBX7[16]; }
        [[nodiscard]] bool RDSEED() const noexcept { return this->EBX7[18]; }
        [[nodiscard]] bool ADX() const noexcept { return this->EBX7[19]; }
        [[nodiscard]] bool AVX512PF() const noexcept { return this->EBX7[26]; }
        [[nodiscard]] bool AVX512ER() const noexcept { return this->EBX7[27]; }
        [[nodiscard]] bool AVX512CD() const noexcept { return this->EBX7[28]; }
        [[nodiscard]] bool SHA() const noexcept { return this->EBX7[29]; }

        [[nodiscard]] bool PREFETCHWT1() const noexcept { return this->ECX7[0]; }

        [[nodiscard]] bool LAHF() const noexcept { return this->ECX81[0]; }
        [[nodiscard]] bool LZCNT() const noexcept { return this->isINTEL && this->ECX81[5]; }
        [[nodiscard]] bool ABM() const noexcept { return this->isAMD && this->ECX81[5]; }
        [[nodiscard]] bool SSE4a() const noexcept { return this->isAMD && this->ECX81[6]; }
        [[nodiscard]] bool XOP() const noexcept { return this->isAMD && this->ECX81[11]; }
        [[nodiscard]] bool TBM() const noexcept { return this->isAMD && this->ECX81[21]; }

        [[nodiscard]] bool SYSCALL() const noexcept { return this->isINTEL && this->EDX81[11]; }
        [[nodiscard]] bool MMXEXT() const noexcept { return this->isAMD && this->EDX81[22]; }
        [[nodiscard]] bool RDTSCP() const noexcept { return this->isINTEL && this->EDX81[27]; }
        [[nodiscard]] bool _3DNOWEXT() const noexcept { return this->isAMD && this->EDX81[30]; }
        [[nodiscard]] bool _3DNOW() const noexcept { return this->isAMD && this->EDX81[31]; }

    public:
        [[nodiscard]] static uint64_t rdtsc() noexcept;

    private:
        std::bitset<32> ECX1;
        std::bitset<32> EDX1;
        std::bitset<32> EBX7;
        std::bitset<32> ECX7;
        std::bitset<32> ECX81;
        std::bitset<32> EDX81;
        std::vector<Register> data;
        std::vector<Register> exData;
    };

}// namespace sese