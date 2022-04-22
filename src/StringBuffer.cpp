#include "sese/StringBuffer.h"
#include "sese/thread/Locker.h"

sese::StringBuffer::StringBuffer(size_t cap) noexcept : AbstractStringBuffer(cap) {
}

sese::StringBuffer::StringBuffer(const char *str) noexcept : AbstractStringBuffer(str) {
}

void sese::StringBuffer::append(char ch) noexcept {
    Locker locker(mutex);
    AbstractStringBuffer::append(ch);
}

void sese::StringBuffer::append(const char *str) noexcept {
    Locker locker(mutex);
    AbstractStringBuffer::append(str);
}

void sese::StringBuffer::clear() noexcept {
    Locker locker(mutex);
    AbstractStringBuffer::clear();
}

void sese::StringBuffer::reverse() noexcept {
    Locker locker(mutex);
    AbstractStringBuffer::reverse();
}

size_t sese::StringBuffer::length() noexcept {
    Locker locker(mutex);
    return AbstractStringBuffer::length();
}

size_t sese::StringBuffer::size() noexcept {
    Locker locker(mutex);
    return AbstractStringBuffer::size();
}

bool sese::StringBuffer::empty() noexcept {
    Locker locker(mutex);
    return AbstractStringBuffer::empty();
}

char sese::StringBuffer::getCharAt(int index) {
    Locker locker(mutex);
    return AbstractStringBuffer::getCharAt(index);
}

void sese::StringBuffer::setChatAt(int index, char ch) {
    Locker locker(mutex);
    AbstractStringBuffer::setChatAt(index, ch);
}

void sese::StringBuffer::delCharAt(int index) {
    Locker locker(mutex);
    AbstractStringBuffer::delCharAt(index);
}

void sese::StringBuffer::del(int start, int end) {
    Locker locker(mutex);
    AbstractStringBuffer::del(start, end);
}

void sese::StringBuffer::insertAt(int index, const char *str) {
    Locker locker(mutex);
    AbstractStringBuffer::insertAt(index, str);
}

void sese::StringBuffer::insertAt(int index, const std::string &str) {
    Locker locker(mutex);
    AbstractStringBuffer::insertAt(index, str);
}

void sese::StringBuffer::trim() noexcept {
    Locker locker(mutex);
    AbstractStringBuffer::trim();
}

std::vector<std::string> sese::StringBuffer::split(const std::string &str) noexcept {
    Locker locker(mutex);
    return AbstractStringBuffer::split(str);
}

std::string sese::StringBuffer::toString() {
    Locker locker(mutex);
    return AbstractStringBuffer::toString();
}
