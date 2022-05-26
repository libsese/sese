#pragma once
#include <sese/Config.h>
#include <sese/thread/Locker.h>
#include <atomic>
#include <functional>
#include <list>

#ifdef _WIN32
#pragma warning(disable : 4251)
#endif

namespace sese {

    class API Recycler;
    class API Recyclable {
    public:
        friend class Recycler;
        explicit Recyclable() noexcept = default;
        virtual void recycle() noexcept;
        [[nodiscard]] bool isRecycled() const noexcept { return _isRecycled; }

    private:
        Recycler *recycler = nullptr;
        bool _isRecycled = true;
    };

    class API Recycler {
    public:
        using FuncNewObject = std::function<Recyclable *()>;
        using ObjectStack = std::list<Recyclable *>;
        using SizeType = size_t;
        using MutexType = std::mutex;

        explicit Recycler(size_t baseSize, const Recycler::FuncNewObject &funcNewObject) noexcept;
        ~Recycler() noexcept;

        Recyclable *get() noexcept;

        template<typename T>
        inline T *getAs() {
            return dynamic_cast<T *>(get());
        }

        // todo 彻底释放部分资源
        void recycle() noexcept;

        void recycle(Recyclable *recyclable) noexcept;

    protected:
        MutexType mutex;
        FuncNewObject newObject;
        SizeType size = 0;
        SizeType available = 0;
        ObjectStack stack;
    };
}// namespace sese