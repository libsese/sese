#include "sese/plugin/Marco.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

class Bye : public Printable {
public:
    void run() override {
        puts("Bye");
    }
};

class Hello : public Printable {
public:
    void run() override {
        puts("Hello");
    }
};

DEFINE_MODULE_INFO(
        .moduleName = "MyModule",
        .versionString = "0.1.0",
        .description = "The module for test."
)

DEFINE_CLASS_FACTORY(
        REGISTER_CLASS("com.kaoru.plugin.test.Bye", Bye),
        REGISTER_CLASS("com.kaoru.plugin.test.Hello", Hello)
)