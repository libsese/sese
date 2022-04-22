/**
 * @file CpuInfo.h
 * @brief Cpu 信息类
 * @author kaoru
 * @date 2022年3月28日
 */
#pragma once
#include "Config.h"

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
        [[nodiscard]] static const std::string &getVendor() noexcept;
        [[nodiscard]] static const std::string &getBrand() noexcept;
        [[nodiscard]] static const std::string &getSerialNumber() noexcept;
        [[nodiscard]] static uint32_t getLogicProcessors() noexcept;

    public:
        [[nodiscard]] static bool SSE3() noexcept;
        [[nodiscard]] static bool PCLMULQDQ() noexcept;
        [[nodiscard]] static bool MONITOR() noexcept;
        [[nodiscard]] static bool SSSE3() noexcept;
        [[nodiscard]] static bool FMA() noexcept;
        [[nodiscard]] static bool CMPXCHG16B() noexcept;
        [[nodiscard]] static bool SSE41() noexcept;
        [[nodiscard]] static bool SSE42() noexcept;
        [[nodiscard]] static bool MOVBE() noexcept;
        [[nodiscard]] static bool POPCNT() noexcept;
        [[nodiscard]] static bool XSAVE() noexcept;
        [[nodiscard]] static bool OSXSAVE() noexcept;
        [[nodiscard]] static bool AVX() noexcept;
        [[nodiscard]] static bool F16C() noexcept;
        [[nodiscard]] static bool RDRAND() noexcept;

        [[nodiscard]] static bool MSR() noexcept;
        [[nodiscard]] static bool CX8() noexcept;
        [[nodiscard]] static bool SEP() noexcept;
        [[nodiscard]] static bool CMOV() noexcept;
        [[nodiscard]] static bool CLFSH() noexcept;
        [[nodiscard]] static bool MMX() noexcept;
        [[nodiscard]] static bool FXSR() noexcept;
        [[nodiscard]] static bool SSE() noexcept;
        [[nodiscard]] static bool SSE2() noexcept;

        [[nodiscard]] static bool FSGSBASE() noexcept;
        [[nodiscard]] static bool BMI1() noexcept;
        [[nodiscard]] static bool HLE() noexcept;
        [[nodiscard]] static bool AVX2() noexcept;
        [[nodiscard]] static bool BMI2() noexcept;
        [[nodiscard]] static bool ERMS() noexcept;
        [[nodiscard]] static bool INVPCID() noexcept;
        [[nodiscard]] static bool RTM() noexcept;
        [[nodiscard]] static bool AVX512F() noexcept;
        [[nodiscard]] static bool RDSEED() noexcept;
        [[nodiscard]] static bool ADX() noexcept;
        [[nodiscard]] static bool AVX512PF() noexcept;
        [[nodiscard]] static bool AVX512ER() noexcept;
        [[nodiscard]] static bool AVX512CD() noexcept;
        [[nodiscard]] static bool SHA() noexcept;

        [[nodiscard]] static bool PREFETCHWT1() noexcept;

        [[nodiscard]] static bool LAHF() noexcept;
        [[nodiscard]] static bool LZCNT() noexcept;
        [[nodiscard]] static bool ABM() noexcept;
        [[nodiscard]] static bool SSE4a() noexcept;
        [[nodiscard]] static bool XOP() noexcept;
        [[nodiscard]] static bool TBM() noexcept;

        [[nodiscard]] static bool SYSCALL() noexcept;
        [[nodiscard]] static bool MMXEXT() noexcept;
        [[nodiscard]] static bool RDTSCP() noexcept;
        [[nodiscard]] static bool _3DNOWEXT() noexcept;
        [[nodiscard]] static bool _3DNOW() noexcept;

        [[nodiscard]] static bool AES() noexcept;
        [[nodiscard]] static uint64_t RDTSC() noexcept;
    };

}// namespace sese