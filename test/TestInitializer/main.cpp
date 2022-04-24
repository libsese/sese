#include "sese/Initializer.h"
#include "sese/record/LogHelper.h"

using sese::Initializer;
using sese::InitiateTask;
using sese::LogHelper;

class Task1 : public InitiateTask {
public:
    Task1() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        LogHelper helper("fINIT");
        helper.debug("loading %s", this->getName().c_str());
        return 0;
    }

    int32_t destroy() noexcept override {
        LogHelper helper("fINIT");
        helper.debug("unloading %s", this->getName().c_str());
        return 0;
    }
};

class Task2 : public InitiateTask {
public:
    Task2() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        LogHelper helper("fINIT");
        helper.debug("loading %s", this->getName().c_str());
        return 0;
    }

    int32_t destroy() noexcept override {
        LogHelper helper("fINIT");
        helper.debug("unloading %s", this->getName().c_str());
        return 0;
    }
};

int main() {
    Initializer::addTask(std::make_shared<Task1>());
    Initializer::addTask(std::make_shared<Task2>());
    return 0;
}