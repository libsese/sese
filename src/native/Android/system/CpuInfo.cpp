#include "system/CpuInfo.h"
#include "native/ARMv8Config.h"
#include <sys/auxv.h>
#include <thread>

namespace sese {
    namespace _android {

        static uint32_t hwcaps;
        static std::string vendor;
        static std::string brand;
        static std::string serialNumber;

        static struct CpuInitStruct {
            CpuInitStruct() {
                int64_t cpuid;
                hwcaps = getauxval(AT_HWCAP);
                asm("mrs %0, MIDR_EL1"
                    : "=r"(cpuid));

                auto vendorId = cpuid >> 24 & 0xFF;
                if (ARMv8VendorNameMap.contains((uint8_t) vendorId)) {
                    vendor = ARMv8VendorNameMap[(uint8_t) vendorId];
                } else {
                    vendor = "UNKNOWN_VENDOR";
                }

                auto pvr = cpuid >> 4 & 0xFF;
                char tempBrand[17];
                sprintf(tempBrand, "%016lX", pvr);
                brand = tempBrand;

                char tempSerialNumber[17];
                sprintf(tempSerialNumber, "%016lX", cpuid);
                serialNumber = tempSerialNumber;
            }
        } cpuInitStruct; /* NOLINT */

    }// namespace _android

    bool CpuInfo::isIntel() noexcept { return false; }
    bool CpuInfo::isAmd() noexcept { return false; }
    bool CpuInfo::isArm() noexcept { return true; }
    const std::string &CpuInfo::getVendor() noexcept { return _android::vendor; }
    const std::string &CpuInfo::getBrand() noexcept { return _android::brand; }
    const std::string &CpuInfo::getSerialNumber() noexcept { return _android::serialNumber; }
    uint32_t CpuInfo::getLogicProcessors() noexcept { return std::thread::hardware_concurrency(); }

#define XX(FLAG_NAME) return _android::hwcaps & (1UL << ARM_v8_FLAG_##FLAG_NAME);
    bool CpuInfo::FP() noexcept { XX(FP) }
    bool CpuInfo::ASIMD() noexcept { XX(ASIMD) }
    bool CpuInfo::EVTSTRM() noexcept { XX(EVTSTRM) }
    bool CpuInfo::AES() noexcept { XX(AES) }
    bool CpuInfo::PMULL() noexcept { XX(PMULL) }
    bool CpuInfo::SHA1() noexcept { XX(SHA1) }
    bool CpuInfo::SHA2() noexcept { XX(SHA2) }
    bool CpuInfo::CRC32() noexcept { XX(CRC32) }

    bool CpuInfo::ATOMICS() noexcept { XX(ATOMICS) }
    bool CpuInfo::FPHP() noexcept { XX(FPHP) }
    bool CpuInfo::ASIMDHP() noexcept { XX(ASIMDHP) }
    bool CpuInfo::CPUID() noexcept { XX(CPUID) }
    bool CpuInfo::ASIMDRDM() noexcept { XX(ASIMDRDM) }
    bool CpuInfo::JSCVT() noexcept { XX(JSCVT) }
    bool CpuInfo::FCMA() noexcept { XX(FCMA) }
    bool CpuInfo::LRCPC() noexcept { XX(LRCPC) }

    bool CpuInfo::DCPOP() noexcept { XX(DCPOP) }
    bool CpuInfo::SHA3() noexcept { XX(SHA3) }
    bool CpuInfo::SM3() noexcept { XX(SM3) }
    bool CpuInfo::SM4() noexcept { XX(SM4) }
    bool CpuInfo::ASIMDDP() noexcept { XX(ASIMDDP) }
    bool CpuInfo::SHA512() noexcept { XX(SHA512) }
    bool CpuInfo::SVE() noexcept { XX(SVE) }
    bool CpuInfo::ASIMDFHM() noexcept { XX(ASIMDFHM) }

    bool CpuInfo::DIT() noexcept { XX(DIT) }
    bool CpuInfo::USCAT() noexcept { XX(USCAT) }
    bool CpuInfo::ILRCPC() noexcept { XX(ILRCPC) }
    bool CpuInfo::FLAGM() noexcept { XX(FLAGM) }
    bool CpuInfo::SSBS() noexcept { XX(SSBS) }
    bool CpuInfo::SB() noexcept { XX(SB) }
    bool CpuInfo::PACA() noexcept { XX(PACA) }
    bool CpuInfo::PACG() noexcept { XX(PACG) }

#undef XX

}// namespace sese