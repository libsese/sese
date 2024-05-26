#include "sese/plugin/Marco.h"

class Printable : public sese::plugin::BaseClass {
public:
    virtual void run() = 0;
};

namespace M {
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
} // namespace M

DEFINE_CLASS_FACTORY(MyFactory, "0.2.0", "The module for test.") {
    REGISTER_CLASS(M::Bye);
    REGISTER_CLASS(M::Hello);
}