#include "sese/plugin/Module.h"
#include "gtest/gtest.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

TEST(TestPlugin, _0) {
    auto m = sese::plugin::Module::open(PATH_TO_MODULE);
    ASSERT_TRUE(m != nullptr);
    printf("module info:\n"
           "\tname: %s\n"
           "\tversion: %s\n"
           "\tdescription: %s\n",
           m->getName(),
           m->getVersion(),
           m->getDescription());

    printf("register class:\n");
    printf("\tid%-28s raw name\n", "");
    auto infoMap = m->getRegisterClassInfo();
    for (const auto &info: infoMap) {
        printf("\t%-28s %s\n", info.first.c_str(), info.second.info->name());
    }

    printf("execute plugin code:\n");
    auto p1 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Bye");
    auto p2 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Hello");
    p1->run();
    p2->run();
}