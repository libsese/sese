#include "sese/Random.h"

using sese::Random;

int main() {
    for (int i = 0; i < 20; i++) {
        printf("%" PRIu64 "\n", Random::next());
    }
    return 0;
}