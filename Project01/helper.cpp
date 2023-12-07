#include "Config.h"

#ifndef RANDOM_SEED
#define RANDOM_SEED -1
#endif // !RANDOM_SEED

namespace global
{
	extern unsigned int random_seed_value = RANDOM_SEED;
}