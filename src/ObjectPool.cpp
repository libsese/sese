#include <sese/ObjectPool.h>

using sese::Recyclable;
using sese::Recycler;

// 可回收类型
void Recyclable::recycle() noexcept {
    // 防止二次回收
    if (!this->_isRecycled) {
        this->recycler->recycle(this);
    }
}

// 回收器 - 对象池核心逻辑
sese::Recycler::Recycler(size_t baseSize, const Recycler::FuncNewObject &funcNewObject) noexcept {
    size = baseSize;
    available = baseSize;
    newObject = funcNewObject;
    for (SizeType i = 0; i < baseSize; i++) {
        auto object = newObject();
        object->recycler = this;
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
    mutex.lock();
    if (available == 0) {
        size++;
        mutex.unlock();
        auto object = newObject();
        object->recycler = this;
        object->_isRecycled = false;
        return object;
    } else {
        available--;
        auto front = stack.front();
        stack.pop_back();
        mutex.unlock();
        front->_isRecycled = false;
        return front;
    }
}

void Recycler::recycle() noexcept {}

void Recycler::recycle(Recyclable *recyclable) noexcept {
    Locker locker(mutex);
    available++;
    recyclable->_isRecycled = true;
    stack.emplace_back(recyclable);
}