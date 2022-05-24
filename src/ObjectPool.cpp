#include <sese/ObjectPool.h>

using sese::Recyclable;
using sese::Recycler;

// 可回收类型
void Recyclable::recycle() noexcept {
    this->recycler->recycle(this);
}

// 回收器 - 对象池核心逻辑
sese::Recycler::Recycler(size_t baseSize, const Recycler::FuncNewObject &funcNewObject) noexcept {
    size = baseSize;
    available = baseSize;
    newObject = funcNewObject;
    for (SizeType i = 0; i < baseSize; i++) {
        auto object = newObject();
        object->setRecycler(this);
        stack.emplace_back(object);
    }
}

Recycler::~Recycler() noexcept {
    Locker locker(mutex);
    for (auto item: stack) {
        delete item;
    }
}

Recyclable *Recycler::get() noexcept {
    Locker locker(mutex);
    if (available == 0) {
        size++;
        auto object = newObject();
        object->setRecycler(this);
        return object;
    } else {
        available--;
        auto front = stack.front();
        stack.pop_back();
        return front;
    }
}

void Recycler::recycle() noexcept {}

void Recycler::recycle(Recyclable *recyclable) noexcept {
    Locker locker(mutex);
    available++;
    stack.emplace_back(recyclable);
}