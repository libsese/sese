#include "sese/text/StringBuilder.h"
#include "sese/record/LogHelper.h"
#include "sese/util/Test.h"
#include <cstdio>

sese::record::LogHelper helper("fSTRING_BUILDER");// NOLINT

int main() {
    auto buffer = new sese::text::StringBuilder(16);
    buffer->append("Hello, World. ");
    buffer->append("This test, for the expansion mechanism.");
    puts(buffer->toString().c_str());
    assert(helper, buffer->getCharAt(13) == ' ', 0);

    buffer->del(0, 13);
    puts(buffer->toString().c_str());

    buffer->insertAt(0, "Hello, World. ");
    puts(buffer->toString().c_str());
    buffer->clear();

    buffer->append("A,B,C,D,E");
    auto str = buffer->split(", ");
    for (const auto &sub: str) {
        puts(sub.c_str());
    }

    buffer->reverse();
    puts(buffer->toString().c_str());

    buffer->delCharAt(5);
    puts(buffer->toString().c_str());
    buffer->clear();

    buffer->append("      Test      ");
    buffer->trim();
    puts(buffer->toString().c_str());
    buffer->clear();

    buffer->append(" Hello  ");
    buffer->trim();
    puts(buffer->toString().c_str());
    buffer->clear();

    buffer->append(" Hi");
    buffer->trim();
    puts(buffer->toString().c_str());
    buffer->clear();

    buffer->append("Bye  ");
    buffer->trim();
    puts(buffer->toString().c_str());
    buffer->clear();

    assert(helper, buffer->setChatAt(100000, 'E'), 0);

    delete buffer;
    return 0;
}