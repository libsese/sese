#include "sese/util/Uuid.h"

#ifdef WIN32
#pragma warning(disable : 4146)
#pragma warning(disable : 4244)
#endif

sese::Uuid::Uuid(uint8_t selfId, uint64_t timestamp, uint8_t r) noexcept {
    this->selfId = selfId;
    this->timestamp = timestamp;
    this->r = r;
}

uint8_t sese::Uuid::getSelfId() const noexcept {
    return selfId;
}

void sese::Uuid::setSelfId(uint8_t i) noexcept {
    Uuid::selfId = i;
}

uint8_t sese::Uuid::getR() const noexcept {
    return r;
}

void sese::Uuid::setR(uint8_t r) noexcept {
    Uuid::r = r;
}

uint64_t sese::Uuid::getTimestamp() const noexcept {
    return timestamp;
}

void sese::Uuid::setTimestamp(uint64_t i) noexcept {
    Uuid::timestamp = i;
}

uint64_t sese::Uuid::toNumber() const noexcept {
    return ((selfId & -1ULL) << 56) | ((r & -1ULL) << 48) | (timestamp & 0x00FF'FFFF'FFFF'FFFF);
}

void sese::Uuid::parse(uint64_t number) noexcept {
    selfId = (number & 0xFF00'0000'0000'0000) >> 56;
    r = (number & 0x00FF'0000'0000'0000) >> 48;
    timestamp = number & 0x0000'FFFF'FFFF'FFFF;
}
