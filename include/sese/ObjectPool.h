/**
 * @file ObjectPool.h
 * @author kaoru
 * @date 2022年6月2日
 * @brief 对象池
 * @version 0.1
 */
#pragma once
#include <sese/Config.h>
#include <sese/thread/Locker.h>
#include <atomic>
#include <functional>
#include <queue>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    /**
     * 可回收对象
     * @tparam T 数据类型
     */
    template<typename T>
    class Recyclable {
    public:
        using Ptr = std::unique_ptr<Recyclable>;

        virtual ~Recyclable() noexcept = default;

        T getValue() const noexcept { return value; }
        void setValue(T v) noexcept { this->value = v; }

    private:
        T value{};
    };

    /**
     * 回收器
     * @tparam T 数据类型
     * @warning 已知在调用 get() 获取对象后不进行回收，会导致 size 基数不断增大并引发相关问题
     */
    template<typename T>
    class Recycler {
    public:
        using SizeType = size_t;
        using MutexType = std::mutex;
        using RecyclableType = Recyclable<T>;
        using ObjectPool = std::queue<typename Recyclable<T>::Ptr>;

        /**
         * 初始化一个对象池
         * @param baseSize 基础大小
         */
        explicit Recycler(SizeType baseSize) noexcept {
            this->baseSize = this->size = this->available = baseSize;
            for (SizeType i = 0; i < baseSize; i++) {
                pool.push(std::make_unique<RecyclableType>());
            }
        }

        /**
         * 从对象池中获取一个可回收对象
         * @return 可回收对象
         */
        typename Recyclable<T>::Ptr get() noexcept {
            mutex.lock();
            if (available == 0) {
                size++;
                mutex.unlock();
                return std::make_unique<RecyclableType>();
            } else {
                available--;
                auto front = std::move(pool.front());
                pool.pop();
                mutex.unlock();
                return front;
            }
        }

        /**
         * @brief 彻底释放部分资源
         * @verbatim
         * 可用对象数量大于总数的 1/4，
         * 则触发回收，回收可用数量的 1/2，
         * 可用对象总数一定不会小于 baseSize
         * @endverbatim
         */
        void recycle() noexcept {
            if (this->available > this->size / 4) {
                Locker locker(this->mutex);
                auto toFree = this->available / 2;
                if (this->size - toFree < this->baseSize) {
                    toFree = this->size - this->baseSize;
                }
                this->available -= toFree;
                this->size -= toFree;
                for (size_t i = 0; i < toFree; i++) {
                    pool.pop();
                }
            }
        }

        /**
         * 回收资源
         * @param recyclable 可回收对象
         * @warning 回收后将会置空原先指针(std::move)
         */
        void recycle(typename RecyclableType::Ptr &recyclable) noexcept {
            Locker locker(mutex);
            available++;
            pool.push(std::move(recyclable));
        }

    private:
        MutexType mutex;
        SizeType baseSize = 0; // 基础大小，不能小于该值
        SizeType size = 0;     // 实际大小
        SizeType available = 0;// 可用大小
        ObjectPool pool;
    };
}// namespace sese