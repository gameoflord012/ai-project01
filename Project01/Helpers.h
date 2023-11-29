#pragma once

#define ROUND_INT(x) ((int)(x + 0.5f))
#define STRING_EQUAL(x, y) (strcmp(x, y) == 0)
#define wrap_fscanf_s(...) { if(fscanf_s(__VA_ARGS__) == 0) { printf("invalid input"); assert(false); exit(0); } }
#define NEW_PRINT_SECTION(s) {printf("\n\n==========[%s]==========", #s);}
using byte = char;
