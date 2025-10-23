#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define ARR_LEN(a) ((int)(sizeof(a)/sizeof((a)[0])))
#define CLAMP(v,minv,maxv) ((v)<(minv)?(minv):((v)>(maxv)?(maxv):(v)))

#endif // COMMON_H

