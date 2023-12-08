#pragma once
#include <cmath>

#include "Config.h"
#include "HashUtils.h"

SEARCH_API struct Position
{
    int x, y, z;

    bool equals(const Position& other) const;
    
    unsigned int operator()(const Position& position) const;

    float distance(const Position& p) const;
    int mahattan_distance(const Position& p);
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