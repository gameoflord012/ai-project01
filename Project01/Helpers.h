#pragma once

#define ROUND_INT(x) ((int)(x + 0.5f))
#define STRING_EQUAL(x, y) (strcmp(x, y) == 0)
#define GET_MASK(X, Y) ((1 << X << 4) | Y)
#define wrap_fscanf_s(...) { if(fscanf_s(__VA_ARGS__) == 0) { printf("invalid input"); assert(false); exit(0); } }

using byte = char;
