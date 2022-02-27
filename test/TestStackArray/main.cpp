#include "util/StackArray.h"
#include <cstdio>

int main() {
    sese::StackArray<int, 4> array;
    array[0] = 10;
    array[3] = 5;

    for(int i = 0; i < array.length(); i++) {
        printf("%d\n", array[i]);
    }

    return 0;
}