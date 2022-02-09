#ifndef UTIL_H
#define UTIL_H

inline int min(int a, int b) {
    if (a <= b) {
        return a;
    } else {
        return b;
    }
}

inline int max(int a, int b) {
    if (a >= b) {
        return a;
    } else {
        return b;
    }
}

// template<typename S>
// auto select_random(const S &s, size_t n);

#endif
