#include "sese/Util.h"
#include "sese/record/FileAppender.h"
#include "sese/record/LogHelper.h"
#include "sese/record/SimpleFormatter.h"
#include "sese/system/CpuInfo.h"

using namespace sese;

LogHelper helper("fCPU_INFO");// NOLINT

void isSupport(const std::string &name, bool isSupport) {
    helper.info("%10s %s",
                name.c_str(),
                isSupport ? "Support" : "Not support");
}

int main() {
    auto logger = getLogger();
    auto formatter = std::make_shared<SimpleFormatter>();
    auto appender = std::make_shared<FileAppender>("CpuInfo.log", formatter);
    logger->addAppender(appender);

    helper.info("%s", CpuInfo::getVendor());
    helper.info("%s", CpuInfo::getBrand());
    helper.info("%s", CpuInfo::getSerialNumber());
    helper.info("%dt", CpuInfo::getLogicProcessors());

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

    return 0;
}