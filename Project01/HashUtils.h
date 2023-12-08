#pragma once

#include <functional>
#include <string>

template <typename T>
inline void hash_combine(unsigned int& s, const T& v);

template <typename T>
inline unsigned int get_hash(const T& v)
{
    return v(v);
}

template<>
inline unsigned int get_hash(const int& v)
{
    return std::hash<int>()(v);
}


template<>
inline unsigned int get_hash(const unsigned int& v)
{
    return std::hash<unsigned int>()(v);
}
template<>
inline unsigned int get_hash(const std::string& v)
{
    return std::hash<std::string>()(v);
}

template<typename T>
inline unsigned int get_hash(const std::vector<T>& v)
{
    unsigned int hash_value = 0;
    for (const T& e : v)
    {
        hash_combine(hash_value, e);
    }
    return hash_value;
}

template <typename T>
inline void hash_combine(unsigned int& s, const T& v)
{
    s ^= get_hash(v)+0x9e3779b9 + (s << 6) + (s >> 2);
}
