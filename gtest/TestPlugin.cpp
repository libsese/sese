#include "sese/plugin/Module.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};


int main() {
    auto m = sese::plugin::Module::open(PROJECT_BINARY_PATH"/gtest/Module.m");
    printf("module info:\n"
           "\tname: %s\n"
           "\tversion: %s\n"
           "\tdescription: %s\n",
           m->getName(),
           m->getVersion(),
           m->getName());

    auto p1 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Bye");
    auto p2 = m->createClassWithIdAs<Printable>("com.kaoru.plugin.test.Hello");
    p1->run();
    p2->run();
}