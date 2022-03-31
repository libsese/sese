#include "Singleton.h"
#include "Util.h"
#include "record/FileAppender.h"
#include "system/CpuInfo.h"

#define FILTER_TEST_CPU_INFO "fCPU_INFO"

using namespace sese;

void isSupport(const std::string &name, bool isSupport) {
    ROOT_INFO(FILTER_TEST_CPU_INFO,
              "%10s %s",
              name.c_str(),
              isSupport ? "Support" : "Not support")
}

int main() {
    auto logger = getLogger();
    auto formatter = std::make_shared<SimpleFormatter>();
    auto appender = std::make_shared<FileAppender>("CpuInfo.log", formatter);
    logger->addAppender(appender);

    ROOT_INFO(FILTER_TEST_CPU_INFO, "%s", CpuInfo::getVendor().c_str())
    ROOT_INFO(FILTER_TEST_CPU_INFO, "%s", CpuInfo::getBrand().c_str())
    ROOT_INFO(FILTER_TEST_CPU_INFO, "%s", CpuInfo::getSerialNumber().c_str())
    if (CpuInfo::isArm()) {
        isSupport("FP", CpuInfo::FP());
        isSupport("ASIMD", CpuInfo::ASIMD());
        isSupport("EVTSTRM", CpuInfo::EVTSTRM());
        isSupport("AES", CpuInfo::AES());
        isSupport("PMULL", CpuInfo::PMULL());
        isSupport("SHA1", CpuInfo::SHA1());
        isSupport("SHA2", CpuInfo::SHA2());
        isSupport("CRC32", CpuInfo::CRC32());

        isSupport("ATOMICS", CpuInfo::ATOMICS());
        isSupport("FPHP", CpuInfo::FPHP());
        isSupport("ASIMDHP", CpuInfo::ASIMDHP());
        isSupport("CPUID", CpuInfo::CPUID());
        isSupport("ASIMDRDM", CpuInfo::ASIMDRDM());
        isSupport("JSCVT", CpuInfo::JSCVT());
        isSupport("FCMA", CpuInfo::FCMA());
        isSupport("LRCPC", CpuInfo::LRCPC());

        isSupport("DCPOP", CpuInfo::DCPOP());
        isSupport("SHA3", CpuInfo::SHA3());
        isSupport("SM3", CpuInfo::SM3());
        isSupport("SM4", CpuInfo::SM4());
        isSupport("ASIMDDP", CpuInfo::ASIMDDP());
        isSupport("SHA512", CpuInfo::SHA512());
        isSupport("SVE", CpuInfo::SVE());
        isSupport("ASIMDFHM", CpuInfo::ASIMDFHM());

        isSupport("DIT", CpuInfo::DIT());
        isSupport("USCAT", CpuInfo::USCAT());
        isSupport("ILRCPC", CpuInfo::ILRCPC());
        isSupport("FLAGM", CpuInfo::FLAGM());
        isSupport("SSBS", CpuInfo::SSBS());
        isSupport("SB", CpuInfo::SB());
        isSupport("PACA", CpuInfo::PACA());
        isSupport("PACG", CpuInfo::PACG());

    } else {
        isSupport("3DNOW", CpuInfo::_3DNOW());
        isSupport("3DNOWEXT", CpuInfo::_3DNOWEXT());
        isSupport("ABM", CpuInfo::ABM());
        isSupport("ADX", CpuInfo::ADX());
        isSupport("AES", CpuInfo::AES());
        isSupport("AVX", CpuInfo::AVX());
        isSupport("AVX2", CpuInfo::AVX2());
        isSupport("AVX512CD", CpuInfo::AVX512CD());
        isSupport("AVX512ER", CpuInfo::AVX512ER());
        isSupport("AVX512F", CpuInfo::AVX512F());
        isSupport("AVX512PF", CpuInfo::AVX512PF());
        isSupport("BMI1", CpuInfo::BMI1());
        isSupport("BMI2", CpuInfo::BMI2());
        isSupport("CLFSH", CpuInfo::CLFSH());
        isSupport("CMPXCHG16B", CpuInfo::CMPXCHG16B());
        isSupport("CX8", CpuInfo::CX8());
        isSupport("ERMS", CpuInfo::ERMS());
        isSupport("F16C", CpuInfo::F16C());
        isSupport("FMA", CpuInfo::FMA());
        isSupport("FSGSBASE", CpuInfo::FSGSBASE());
        isSupport("FXSR", CpuInfo::FXSR());
        isSupport("HLE", CpuInfo::HLE());
        isSupport("INVPCID", CpuInfo::INVPCID());
        isSupport("LAHF", CpuInfo::LAHF());
        isSupport("LZCNT", CpuInfo::LZCNT());
        isSupport("MMX", CpuInfo::MMX());
        isSupport("MMXEXT", CpuInfo::MMXEXT());
        isSupport("MONITOR", CpuInfo::MONITOR());
        isSupport("MOVBE", CpuInfo::MOVBE());
        isSupport("MSR", CpuInfo::MSR());
        isSupport("OSXSAVE", CpuInfo::OSXSAVE());
        isSupport("PCLMULQDQ", CpuInfo::PCLMULQDQ());
        isSupport("POPCNT", CpuInfo::POPCNT());
        isSupport("PREFETCHWT1", CpuInfo::PREFETCHWT1());
        isSupport("RDRAND", CpuInfo::RDRAND());
        isSupport("RDSEED", CpuInfo::RDSEED());
        isSupport("RDTSCP", CpuInfo::RDTSCP());
        isSupport("RTM", CpuInfo::RTM());
        isSupport("SEP", CpuInfo::SEP());
        isSupport("CMOV", CpuInfo::CMOV());
        isSupport("SHA", CpuInfo::SHA());
        isSupport("SSE", CpuInfo::SSE());
        isSupport("SSE2", CpuInfo::SSE2());
        isSupport("SSE3", CpuInfo::SSE3());
        isSupport("SSE4.1", CpuInfo::SSE41());
        isSupport("SSE4.2", CpuInfo::SSE42());
        isSupport("SSE4a", CpuInfo::SSE4a());
        isSupport("SSSE3", CpuInfo::SSSE3());
        isSupport("SYSCALL", CpuInfo::SYSCALL());
        isSupport("TBM", CpuInfo::TBM());
        isSupport("XOP", CpuInfo::XOP());
        isSupport("XSAVE", CpuInfo::XSAVE());
    }
    return 0;
}