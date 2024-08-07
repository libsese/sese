#include <sese/res/Resource.h>
#include <sese/res/Marco.h>

// GCOVR_EXCL_START

sese::res::Resource::Resource(std::string name, const void *buf, size_t size)
    : name(std::move(name)), buf(buf), size(size) {}

// GCOVR_EXCL_STOP

sese::res::ResourceStream::Ptr sese::res::Resource::getStream() {
    return SESE_MAKE_RES_STREAM(buf, size);
}