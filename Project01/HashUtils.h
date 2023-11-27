#pragma once

#include <functional>

template <class T>
inline void hash_combine(std::size_t& s, const T& v)
{
    s ^= std::hash<T>{}(v)+0x9e3779b9 + (s << 6) + (s >> 2);
}
