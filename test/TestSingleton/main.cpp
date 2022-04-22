#include "sese/Singleton.h"

int main() {
    auto i = sese::SingletonPtr<int>::getInstance();
    *i = 10;
    auto j = sese::SingletonPtr<int>::getInstance();


    auto h = sese::Singleton<int>::getInstance();
    *h = 20;
    auto k = sese::Singleton<int>::getInstance();

    return 0;
}