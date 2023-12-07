#pragma once
#include <cmath>

#include "HashUtils.h"

struct Position
{
    int x, y, z;

    bool equals(const Position& other) const {
        return x == other.x and y == other.y and z == other.z;
    }

    unsigned int operator()(const Position& position) const {
        unsigned int hashResult = 0;

        return hashResult;
    }

    float distance(const Position& p) const 
    {
        return std::sqrt(
            std::pow(p.x - x, 2) +
            std::pow(p.y - y, 2) +
            std::pow(p.z - z, 2));
    }

    int mahattan_distance(const Position& p)
    {
        return
            std::abs(p.x - x) +
            std::abs(p.y - y) +
            std::abs(p.z - z);
    }
};

namespace std
{
    template<>
    struct hash<Position>
    {
        unsigned int operator()(const Position& p) const {
            unsigned int hashResult = 0;
            hash_combine(hashResult, p.x);
            hash_combine(hashResult, p.y);
            hash_combine(hashResult, p.z);
            return hashResult;
        }
    };
}