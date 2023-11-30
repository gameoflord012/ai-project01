#pragma once
#include <cmath>

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

    float distance(const Position& p) const 
    {
        return std::sqrt(
            std::pow(p.x - x, 2) +
            std::pow(p.y - y, 2) +
            std::pow(p.z - z, 2));
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