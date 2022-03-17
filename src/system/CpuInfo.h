#pragma once
#include "Config.h"
#include "util/NotInstantiable.h"
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

    public:
        [[nodiscard]] bool isIntel() const { return isINTEL; }
        [[nodiscard]] bool isAmd() const { return isAMD; }
        [[nodiscard]] const std::string &getVendor() const { return vendor; }
        [[nodiscard]] const std::string &getBrand() const { return brand; }

        [[nodiscard]] bool SSE3() { return this->ECX1[0]; }
        [[nodiscard]] bool PCLMULQDQ() { return this->ECX1[1]; }
        [[nodiscard]] bool MONITOR() { return this->ECX1[3]; }
        [[nodiscard]] bool SSSE3() { return this->ECX1[9]; }
        [[nodiscard]] bool FMA() { return this->ECX1[12]; }
        [[nodiscard]] bool CMPXCHG16B() { return this->ECX1[13]; }
        [[nodiscard]] bool SSE41() { return this->ECX1[19]; }
        [[nodiscard]] bool SSE42() { return this->ECX1[20]; }
        [[nodiscard]] bool MOVBE() { return this->ECX1[22]; }
        [[nodiscard]] bool POPCNT() { return this->ECX1[23]; }
        [[nodiscard]] bool AES() { return this->ECX1[25]; }
        [[nodiscard]] bool XSAVE() { return this->ECX1[26]; }
        [[nodiscard]] bool OSXSAVE() { return this->ECX1[27]; }
        [[nodiscard]] bool AVX() { return this->ECX1[28]; }
        [[nodiscard]] bool F16C() { return this->ECX1[29]; }
        [[nodiscard]] bool RDRAND() { return this->ECX1[30]; }

        [[nodiscard]] bool MSR() { return this->EDX1[5]; }
        [[nodiscard]] bool CX8() { return this->EDX1[8]; }
        [[nodiscard]] bool SEP() { return this->EDX1[11]; }
        [[nodiscard]] bool CMOV() { return this->EDX1[15]; }
        [[nodiscard]] bool CLFSH() { return this->EDX1[19]; }
        [[nodiscard]] bool MMX() { return this->EDX1[23]; }
        [[nodiscard]] bool FXSR() { return this->EDX1[24]; }
        [[nodiscard]] bool SSE() { return this->EDX1[25]; }
        [[nodiscard]] bool SSE2() { return this->EDX1[26]; }

        [[nodiscard]] bool FSGSBASE() { return this->EBX7[0]; }
        [[nodiscard]] bool BMI1() { return this->EBX7[3]; }
        [[nodiscard]] bool HLE() { return this->isINTEL && this->EBX7[4]; }
        [[nodiscard]] bool AVX2() { return this->EBX7[5]; }
        [[nodiscard]] bool BMI2() { return this->EBX7[8]; }
        [[nodiscard]] bool ERMS() { return this->EBX7[9]; }
        [[nodiscard]] bool INVPCID() { return this->EBX7[10]; }
        [[nodiscard]] bool RTM() { return this->isINTEL && this->EBX7[11]; }
        [[nodiscard]] bool AVX512F() { return this->EBX7[16]; }
        [[nodiscard]] bool RDSEED() { return this->EBX7[18]; }
        [[nodiscard]] bool ADX() { return this->EBX7[19]; }
        [[nodiscard]] bool AVX512PF() { return this->EBX7[26]; }
        [[nodiscard]] bool AVX512ER() { return this->EBX7[27]; }
        [[nodiscard]] bool AVX512CD() { return this->EBX7[28]; }
        [[nodiscard]] bool SHA() { return this->EBX7[29]; }

        [[nodiscard]] bool PREFETCHWT1() { return this->ECX7[0]; }

        [[nodiscard]] bool LAHF() { return this->ECX81[0]; }
        [[nodiscard]] bool LZCNT() { return this->isINTEL && this->ECX81[5]; }
        [[nodiscard]] bool ABM() { return this->isAMD && this->ECX81[5]; }
        [[nodiscard]] bool SSE4a() { return this->isAMD && this->ECX81[6]; }
        [[nodiscard]] bool XOP() { return this->isAMD && this->ECX81[11]; }
        [[nodiscard]] bool TBM() { return this->isAMD && this->ECX81[21]; }

        [[nodiscard]] bool SYSCALL() { return this->isINTEL && this->EDX81[11]; }
        [[nodiscard]] bool MMXEXT() { return this->isAMD && this->EDX81[22]; }
        [[nodiscard]] bool RDTSCP() { return this->isINTEL && this->EDX81[27]; }
        [[nodiscard]] bool _3DNOWEXT() { return this->isAMD && this->EDX81[30]; }
        [[nodiscard]] bool _3DNOW() { return this->isAMD && this->EDX81[31]; }

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