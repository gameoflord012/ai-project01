#pragma once

#include <algorithm>

#define ROUND_INT(x) ((int)(x + 0.5f))
#define STRING_EQUAL(x, y) (strcmp(x, y) == 0)
#define wrap_fscanf_s(...) { if(fscanf_s(__VA_ARGS__) == 0) { printf("invalid input"); assert(false); exit(0); } }
#define NEW_PRINT_SECTION(s) {printf("\n\n==========[%s]==========", #s);}
#define FIRST_ON_BIT_POSITION(x) (ROUND_INT(log2(x)))
#define COUNT(ar, e) ((std::count(ar.begin(), ar.end(), e)))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
using byte = char;
