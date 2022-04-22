#include "sese/Test.h"
#include "sese/record/LogHelper.h"

using sese::Test;
using sese::LogHelper;

void Test::assert(LogHelper log, bool expr, const std::function<void()>& callback) {
    if(!expr){
        log.error(backtrace2String(5, "Backtrace ").c_str());
        callback();
    }
}