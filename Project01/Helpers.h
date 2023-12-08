#pragma once

#include "Config.h"
#include "HashUtils.h"

#include <algorithm>
#include <ostream>
#include <string>
#include <chrono>

#define ROUND_INT(x) ((int)(x + 0.5f))
#define STRING_EQUAL(x, y) (strcmp(x, y) == 0)
#define wrap_fscanf_s(...) { if(fscanf_s(__VA_ARGS__) == 0) { printf("invalid input"); assert(false); exit(0); } }
#define NEW_PRINT_SECTION(s) {printf("\n\n==========[%s]==========", #s);}
#define FIRST_ON_BIT_POSITION(x) (ROUND_INT(log2(x)))
#define COUNT(ar, e) ((std::count(ar.begin(), ar.end(), e)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

namespace global
{
	extern unsigned int random_seed_value;
}

template<typename T>
inline unsigned generate_seed(const T& v)
{
	if (global::random_seed_value == -1)
	{
		global::random_seed_value = std::chrono::system_clock::now().time_since_epoch().count();
	}

	unsigned int seed = global::random_seed_value;
	hash_combine(seed, v);
	return seed;
}

using byte = char;
using uint = unsigned int;
