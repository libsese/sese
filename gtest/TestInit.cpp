#include <sese/record/LogHelper.h>
#include <sese/util/Initializer.h>

#include <gtest/gtest.h>

class Task1 : public sese::InitiateTask {
public:
    Task1() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        sese::record::LogHelper::d("loading %s", this->getName().c_str());
        return 0;
    }

    int32_t destroy() noexcept override {
        sese::record::LogHelper::d("unloading %s", this->getName().c_str());
        return -1;
    }
};

class Task2 : public sese::InitiateTask {
public:
    Task2() : InitiateTask(__FUNCTION__) {}

    int32_t init() noexcept override {
        sese::record::LogHelper::d("loading %s", this->getName().c_str());
        return -1;
    }

    int32_t destroy() noexcept override {
        sese::record::LogHelper::d("unloading %s", this->getName().c_str());
        return 0;
    }
};

TEST(TestInit, _0) {
    sese::Initializer::getInitializer();
    sese::Initializer::addTask<Task1>();
    sese::Initializer::addTask(std::make_unique<Task2>());
}