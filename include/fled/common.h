#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* define our version */
#define FLED_VERSION "0.0.1"

#define CTRL_KEY(k) ((k) & 0x1f)

/* exit the program after an error
 * clear the screen before we exit
 * it is explained in output.c
 * we can't do this atexxit because that also clears the error message
 * printed out by the perror function
 */
#define DIE(s) {\
    write(STDOUT_FILENO, "\x1b[2J", 4);\
    write(STDOUT_FILENO, "\x1b[H", 3);\
    perror(s);\
    exit(1);\
}

/* exit the program normally
 * clear the screen before a successful exit as well
 */
#define EXIT() {\
    write(STDOUT_FILENO, "\x1b[2J", 4);\
    write(STDOUT_FILENO, "\x1b[H", 3);\
    exit(0);\
}

typedef struct fled_config {
    struct termios orig_term; /* terminal information at the start */
    int rows, cols; /* terminal size */
    int curx, cury; /* cursor position */
} fled_config_t;

typedef enum editor_keys {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY
} editor_keys_t;

/* a struct containing global definitions
 * defined in main.c
 */
extern fled_config_t* E;

#endif
