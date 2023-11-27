#pragma once
#include "HashUtils.h"

struct Position
{
    int x, y, z;

    bool equals(const Position& other) const {
        return x == other.x and y == other.y and z == other.z;
    }

    std::size_t operator()(const Position& position) const {
        size_t hashResult = 0;

        return hashResult;
    }
};

namespace std
{
    template<>
    struct hash<Position>
    {
        std::size_t operator()(const Position& p) const {
            size_t hashResult = 0;
            hash_combine(hashResult, p.x);
            hash_combine(hashResult, p.y);
            hash_combine(hashResult, p.z);
            return hashResult;
        }
    };
}