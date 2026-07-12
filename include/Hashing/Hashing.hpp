#pragma once

#include <functional>
namespace Spelt {
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v) {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }

    template <class T>
    inline std::size_t hash(const T& v){
        static std::hash<T> hasher;
        return hasher(v);
    }
}
