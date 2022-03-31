/**
 * @file CpuInfo.h
 * @brief Cpu 信息类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Config.h"
#include <string>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /**
     * @brief Cpu 信息类
     */
    class API CpuInfo {
    public:
        [[nodiscard]] static bool isIntel() noexcept;
        [[nodiscard]] static bool isAmd() noexcept;
        [[nodiscard]] static bool isArm() noexcept;
        [[nodiscard]] static const std::string &getVendor() noexcept;
        [[nodiscard]] static const std::string &getBrand() noexcept;
        [[nodiscard]] static const std::string &getSerialNumber() noexcept;

#if defined SESE_ARCH_X64
#define FUNCTION_X64 noexcept;
#define FUNCTION_ARM64 { return false; }
#elif defined SESE_ARCH_ARM64
#define FUNCTION_X64 { return false; }
#define FUNCTION_ARM64 noexcept;
#endif

#pragma region X64专有函数
    public:
        [[nodiscard]] static bool SSE3() FUNCTION_X64
        [[nodiscard]] static bool PCLMULQDQ() FUNCTION_X64
        [[nodiscard]] static bool MONITOR() FUNCTION_X64
        [[nodiscard]] static bool SSSE3() FUNCTION_X64
        [[nodiscard]] static bool FMA() FUNCTION_X64
        [[nodiscard]] static bool CMPXCHG16B() FUNCTION_X64
        [[nodiscard]] static bool SSE41() FUNCTION_X64
        [[nodiscard]] static bool SSE42() FUNCTION_X64
        [[nodiscard]] static bool MOVBE() FUNCTION_X64
        [[nodiscard]] static bool POPCNT() FUNCTION_X64
        [[nodiscard]] static bool XSAVE() FUNCTION_X64
        [[nodiscard]] static bool OSXSAVE() FUNCTION_X64
        [[nodiscard]] static bool AVX() FUNCTION_X64
        [[nodiscard]] static bool F16C() FUNCTION_X64
        [[nodiscard]] static bool RDRAND() FUNCTION_X64

        [[nodiscard]] static bool MSR() FUNCTION_X64
        [[nodiscard]] static bool CX8() FUNCTION_X64
        [[nodiscard]] static bool SEP() FUNCTION_X64
        [[nodiscard]] static bool CMOV() FUNCTION_X64
        [[nodiscard]] static bool CLFSH() FUNCTION_X64
        [[nodiscard]] static bool MMX() FUNCTION_X64
        [[nodiscard]] static bool FXSR() FUNCTION_X64
        [[nodiscard]] static bool SSE() FUNCTION_X64
        [[nodiscard]] static bool SSE2() FUNCTION_X64

        [[nodiscard]] static bool FSGSBASE() FUNCTION_X64
        [[nodiscard]] static bool BMI1() FUNCTION_X64
        [[nodiscard]] static bool HLE() FUNCTION_X64
        [[nodiscard]] static bool AVX2() FUNCTION_X64
        [[nodiscard]] static bool BMI2() FUNCTION_X64
        [[nodiscard]] static bool ERMS() FUNCTION_X64
        [[nodiscard]] static bool INVPCID() FUNCTION_X64
        [[nodiscard]] static bool RTM() FUNCTION_X64
        [[nodiscard]] static bool AVX512F() FUNCTION_X64
        [[nodiscard]] static bool RDSEED() FUNCTION_X64
        [[nodiscard]] static bool ADX() FUNCTION_X64
        [[nodiscard]] static bool AVX512PF() FUNCTION_X64
        [[nodiscard]] static bool AVX512ER() FUNCTION_X64
        [[nodiscard]] static bool AVX512CD() FUNCTION_X64
        [[nodiscard]] static bool SHA() FUNCTION_X64

        [[nodiscard]] static bool PREFETCHWT1() FUNCTION_X64

        [[nodiscard]] static bool LAHF() FUNCTION_X64
        [[nodiscard]] static bool LZCNT() FUNCTION_X64
        [[nodiscard]] static bool ABM() FUNCTION_X64
        [[nodiscard]] static bool SSE4a() FUNCTION_X64
        [[nodiscard]] static bool XOP() FUNCTION_X64
        [[nodiscard]] static bool TBM() FUNCTION_X64

        [[nodiscard]] static bool SYSCALL() FUNCTION_X64
        [[nodiscard]] static bool MMXEXT() FUNCTION_X64
        [[nodiscard]] static bool RDTSCP() FUNCTION_X64
        [[nodiscard]] static bool _3DNOWEXT() FUNCTION_X64
        [[nodiscard]] static bool _3DNOW() FUNCTION_X64

        [[nodiscard]] static uint64_t RDTSC() FUNCTION_X64
#pragma endregion

#pragma region ARM64专有函数
    public:
        [[nodiscard]] static bool FP() FUNCTION_ARM64
        [[nodiscard]] static bool ASIMD() FUNCTION_ARM64
        [[nodiscard]] static bool EVTSTRM() FUNCTION_ARM64
        [[nodiscard]] static bool PMULL() FUNCTION_ARM64
        [[nodiscard]] static bool SHA1() FUNCTION_ARM64
        [[nodiscard]] static bool SHA2() FUNCTION_ARM64
        [[nodiscard]] static bool CRC32() FUNCTION_ARM64

        [[nodiscard]] static bool ATOMICS() FUNCTION_ARM64
        [[nodiscard]] static bool FPHP() FUNCTION_ARM64
        [[nodiscard]] static bool ASIMDHP() FUNCTION_ARM64
        [[nodiscard]] static bool CPUID() FUNCTION_ARM64
        [[nodiscard]] static bool ASIMDRDM() FUNCTION_ARM64
        [[nodiscard]] static bool JSCVT() FUNCTION_ARM64
        [[nodiscard]] static bool FCMA() FUNCTION_ARM64
        [[nodiscard]] static bool LRCPC() FUNCTION_ARM64

        [[nodiscard]] static bool DCPOP() FUNCTION_ARM64
        [[nodiscard]] static bool SHA3() FUNCTION_ARM64
        [[nodiscard]] static bool SM3() FUNCTION_ARM64
        [[nodiscard]] static bool SM4() FUNCTION_ARM64
        [[nodiscard]] static bool ASIMDDP() FUNCTION_ARM64
        [[nodiscard]] static bool SHA512() FUNCTION_ARM64
        [[nodiscard]] static bool SVE() FUNCTION_ARM64
        [[nodiscard]] static bool ASIMDFHM() FUNCTION_ARM64

        [[nodiscard]] static bool DIT() FUNCTION_ARM64
        [[nodiscard]] static bool USCAT() FUNCTION_ARM64
        [[nodiscard]] static bool ILRCPC() FUNCTION_ARM64
        [[nodiscard]] static bool FLAGM() FUNCTION_ARM64
        [[nodiscard]] static bool SSBS() FUNCTION_ARM64
        [[nodiscard]] static bool SB() FUNCTION_ARM64
        [[nodiscard]] static bool PACA() FUNCTION_ARM64
        [[nodiscard]] static bool PACG() FUNCTION_ARM64
#pragma endregion

#if defined(SESE_ARCH_X64) || defined(SESE_ARCH_ARM64)
#undef FUNCTION_X64
#undef FUNCTION_ARM64
#endif

    public:
        [[nodiscard]] static bool AES() noexcept;
    };

}// namespace sese