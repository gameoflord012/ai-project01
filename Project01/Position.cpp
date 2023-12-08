#include "Position.h"

bool Position::equals(const Position& other) const {
    return x == other.x and y == other.y and z == other.z;
}

unsigned int Position::operator()(const Position& position) const {
    unsigned int hashResult = 0;

    return hashResult;
}

float Position::distance(const Position& p) const
{
    return std::sqrt(
        std::pow(p.x - x, 2) +
        std::pow(p.y - y, 2) +
        std::pow(p.z - z, 2));
}

int Position::mahattan_distance(const Position& p)
{
    return
        std::abs(p.x - x) +
        std::abs(p.y - y) +
        std::abs(p.z - z);
}