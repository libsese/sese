#include "util/Array.h"
#include <cstdio>

int main() {
    sese::Array<int, 4> array;
    array[0] = 10;
    array[3] = 5;

    for (int i = 0; i < array.length(); i++) {
        printf("%d\n", array[i]);
    }

    try {
        array[4] = 0;
    } catch (sese::Exception &exception) {
        puts(exception.what());
    }

    return 0;
}