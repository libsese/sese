#include "Util.h"
#include "util/Random.h"

using sese::Random;

int main() {
    for (int i = 0; i < 1E2; i++) {
        ROOT_INFO("%f", Random::randDouble())
    }

    for (int i = 0; i < 1E2; i++) {
        ROOT_INFO("%d", Random::randInt())
    }

    return 0;
}