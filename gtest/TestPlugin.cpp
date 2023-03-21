#include "sese/plugin/Module.h"
#include "gtest/gtest.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

TEST(TestPlugin, _0) {
    auto m = sese::plugin::Module::open(PROJECT_BINARY_PATH"/gtest/Module.m");
    ASSERT_TRUE(m != nullptr);
    printf("module info:\n"
           "\tname: %s\n"
           "\tversion: %s\n"
           "\tdescription: %s\n",
           m->getName(),
           m->getVersion(),
           m->getDescription());

    auto p1 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Bye");
    auto p2 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Hello");
    p1->run();
    p2->run();
}