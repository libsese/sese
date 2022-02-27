#include "util/HeapArray.h"
#include <cstdio>

int main() {
    auto *array = new sese::HeapArray<int>(4);
    (*array)[0] = 10;
    (*array)[3] = 5;

    for(int i = 0; i < array->length(); i++) {
        printf("%d\n", (*array)[i]);
    }

    return 0;
}