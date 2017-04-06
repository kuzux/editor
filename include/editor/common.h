#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>

#define CTRL_KEY(k) ((k) & 0x1f)

/* exit the program after an error */
#define die(s) { perror(s); exit(1); }

#endif
