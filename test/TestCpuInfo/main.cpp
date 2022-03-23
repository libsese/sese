#include "Util.h"
#include "record/FileAppender.h"
#include "system/CpuInfo.h"
#include "Singleton.h"

using namespace sese;

void isSupport(const std::string &name, bool isSupport) {
    ROOT_INFO("%10s %s",
              name.c_str(),
              isSupport ? "Support" : "Not support")
}

int main() {
    auto logger = getLogger();
    auto formatter = std::make_shared<SimpleFormatter>();
    auto appender = std::make_shared<FileAppender>("CpuInfo.log", formatter);
    logger->addAppender(appender);

    auto cpuInfo = Singleton<CpuInfo>();
    ROOT_INFO("%s", cpuInfo.getInstance()->getVendor().c_str())
    ROOT_INFO("%s", cpuInfo.getInstance()->getBrand().c_str())
    ROOT_INFO("%s", cpuInfo.getInstance()->getSerialNumber().c_str())

    isSupport("3DNOW", cpuInfo.getInstance()->_3DNOW());
    isSupport("3DNOWEXT", cpuInfo.getInstance()->_3DNOWEXT());
    isSupport("ABM", cpuInfo.getInstance()->ABM());
    isSupport("ADX", cpuInfo.getInstance()->ADX());
    isSupport("AES", cpuInfo.getInstance()->AES());
    isSupport("AVX", cpuInfo.getInstance()->AVX());
    isSupport("AVX2", cpuInfo.getInstance()->AVX2());
    isSupport("AVX512CD", cpuInfo.getInstance()->AVX512CD());
    isSupport("AVX512ER", cpuInfo.getInstance()->AVX512ER());
    isSupport("AVX512F", cpuInfo.getInstance()->AVX512F());
    isSupport("AVX512PF", cpuInfo.getInstance()->AVX512PF());
    isSupport("BMI1", cpuInfo.getInstance()->BMI1());
    isSupport("BMI2", cpuInfo.getInstance()->BMI2());
    isSupport("CLFSH", cpuInfo.getInstance()->CLFSH());
    isSupport("CMPXCHG16B", cpuInfo.getInstance()->CMPXCHG16B());
    isSupport("CX8", cpuInfo.getInstance()->CX8());
    isSupport("ERMS", cpuInfo.getInstance()->ERMS());
    isSupport("F16C", cpuInfo.getInstance()->F16C());
    isSupport("FMA", cpuInfo.getInstance()->FMA());
    isSupport("FSGSBASE", cpuInfo.getInstance()->FSGSBASE());
    isSupport("FXSR", cpuInfo.getInstance()->FXSR());
    isSupport("HLE", cpuInfo.getInstance()->HLE());
    isSupport("INVPCID", cpuInfo.getInstance()->INVPCID());
    isSupport("LAHF", cpuInfo.getInstance()->LAHF());
    isSupport("LZCNT", cpuInfo.getInstance()->LZCNT());
    isSupport("MMX", cpuInfo.getInstance()->MMX());
    isSupport("MMXEXT", cpuInfo.getInstance()->MMXEXT());
    isSupport("MONITOR", cpuInfo.getInstance()->MONITOR());
    isSupport("MOVBE", cpuInfo.getInstance()->MOVBE());
    isSupport("MSR", cpuInfo.getInstance()->MSR());
    isSupport("OSXSAVE", cpuInfo.getInstance()->OSXSAVE());
    isSupport("PCLMULQDQ", cpuInfo.getInstance()->PCLMULQDQ());
    isSupport("POPCNT", cpuInfo.getInstance()->POPCNT());
    isSupport("PREFETCHWT1", cpuInfo.getInstance()->PREFETCHWT1());
    isSupport("RDRAND", cpuInfo.getInstance()->RDRAND());
    isSupport("RDSEED", cpuInfo.getInstance()->RDSEED());
    isSupport("RDTSCP", cpuInfo.getInstance()->RDTSCP());
    isSupport("RTM", cpuInfo.getInstance()->RTM());
    isSupport("SEP", cpuInfo.getInstance()->SEP());
    isSupport("CMOV", cpuInfo.getInstance()->CMOV());
    isSupport("SHA", cpuInfo.getInstance()->SHA());
    isSupport("SSE", cpuInfo.getInstance()->SSE());
    isSupport("SSE2", cpuInfo.getInstance()->SSE2());
    isSupport("SSE3", cpuInfo.getInstance()->SSE3());
    isSupport("SSE4.1", cpuInfo.getInstance()->SSE41());
    isSupport("SSE4.2", cpuInfo.getInstance()->SSE42());
    isSupport("SSE4a", cpuInfo.getInstance()->SSE4a());
    isSupport("SSSE3", cpuInfo.getInstance()->SSSE3());
    isSupport("SYSCALL", cpuInfo.getInstance()->SYSCALL());
    isSupport("TBM", cpuInfo.getInstance()->TBM());
    isSupport("XOP", cpuInfo.getInstance()->XOP());
    isSupport("XSAVE", cpuInfo.getInstance()->XSAVE());

    return 0;
}