#include <sese/text/Algorithm.h>
#include <sese/text/SString.h>
#include <cstring>
#include <vector>
#ifdef _WIN32
#pragma warning(disable : 4267)
#endif

static std::vector<int> getNext(const char *str) {
    auto len = strlen(str);
    std::vector<int> next(len, 0);
    for (int i = 1; i < len; i++) {
        int k = next[i - 1];
        while (k > 0 && str[i] != str[k]) {
            k = next[k - 1];
        }
        if (str[i] == str[k]) {
            k++;
        }
        next[i] = k;
    }
    return next;
}

int sstr::KMP(const char *str, const char *sub) {
    std::vector<int> next = getNext(str);

    int k = 0;
    int res = -1;
    for (int i = 0; i < (int) strlen(str); i++) {
        while (k > 0 && str[i] != sub[k]) {
            k = next[k - 1];
        }
        if (str[i] == sub[k]) {
            k++;
        }
        if (k == (int) strlen(sub)) {
            res = i - k + 1;
            k = next[k - 1];
        }
    }
    return res;
}

static int dist(const char *t, char ch) {
    auto len = strlen(t);
    int i = len - 1;
    if (ch == t[i])
        return len;
    i--;
    while (i >= 0) {
        if (ch == t[i])
            return len - 1 - i;
        else
            i--;
    }
    return len;
}


int sstr::BM(const char *str, const char *sub) {
    auto n = strlen(str);
    auto m = strlen(sub);
    int i = m - 1;
    int j = m - 1;
    while (j >= 0 && i < n) {
        if (str[i] == sub[j]) {
            i--;
            j--;
        } else {
            i += dist(sub, str[i]);
            j = m - 1;
        }
    }
    if (j < 0) {
        return i + 1;
    }
    return -1;
}

size_t strlen(const uint32_t *str) {    
    const uint32_t *p = str;
    size_t size = 0;
    while(true) {
        if (0 != *p) {
            size++;
            p++;
        } else {
            break;
        }
    }

    return size;
}

static int dist(std::vector<sstr::SChar> &t, uint32_t ch) {
    auto len = t.size();
    int i = len - 1;
    if (ch == (uint32_t) t[i])
        return len;
    i--;
    while (i >= 0) {
        if (ch == (uint32_t) t[i])
            return len - 1 - i;
        else
            i--;
    }
    return len;
}

int sstr::BM(const uint32_t *str, size_t size, std::vector<SChar> &sub) {
    auto n = size;
    auto m = sub.size();
    int i = m - 1;
    int j = m - 1;
    while (j >= 0 && i < n) {
        if (str[i] == (uint32_t) sub[j]) {
            i--;
            j--;
        } else {
            i += dist(sub, str[i]);
            j = m - 1;
        }
    }
    if (j < 0) {
        return i + 1;
    }
    return -1;
}

int sstr::NORMAL(const char *str, const char *sub) {
    auto p = strstr(str, sub);
    return p ? (int) (p - str) : -1;
}