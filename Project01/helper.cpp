#include "Config.h"
#include "Helpers.h"

#ifndef RANDOM_SEED
#define RANDOM_SEED -1
#endif // !RANDOM_SEED

namespace global
{
	unsigned int random_seed_value = RANDOM_SEED;
}