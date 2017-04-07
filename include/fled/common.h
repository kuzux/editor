#ifndef _COMMON_H
#define _COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

#include <fled/rows.h>

/* Define our version */
#define FLED_VERSION "0.0.1"
#define DEBUG 1

#define CTRL_KEY(k) ((k) & 0x1f)

/* Exit the program after an error
 * clear the screen before we exit
 * we can't do this atexit because that also clears the error message
 * printed out by the perror function.
 * Also note that the comments are stripped before the preprocessor
 * runs, or at least in the C99 standard it does, so we can safely
 * put comments in our preprocessor definition (provided we put a 
 * backslash at the end of the comment)
 */
#define DIE(s) {\
    /* Clear the entire screen
     * \x1b[ : start an escape sequence
     * 2 : entire screen
     * J : clear
     */\
    write(STDOUT_FILENO, "\x1b[2J", 4);\
\
    /* Move the cursor to an off-screen position 
     * The default arguments are 0;0
     */\
    write(STDOUT_FILENO, "\x1b[H", 3);\
\
    perror(s);\
    exit(1);\
}

/* Append to our debug log
 * No need for this define to do anything if debug is off
 */
#define DEBUG_LOG(msg) {\
    if(DEBUG) {\
        fprintf(E->debug_log, "%s\n", (msg));\
        fflush(E->debug_log);\
    }\
}
#define DEBUG_LOGF(format, ...) {\
    if(DEBUG) {\
        fprintf(E->debug_log, format, __VA_ARGS__);\
        fflush(E->debug_log);\
    }\
}
/* Exit the program normally
 * clear the screen before a successful exit as well
 */
#define EXIT() {\
    write(STDOUT_FILENO, "\x1b[2J", 4);\
    write(STDOUT_FILENO, "\x1b[H", 3);\
    exit(0);\
}

typedef struct fled_config {
    /* Terminal information at the start
     * a system-defined termios structure
     */
    struct termios orig_term;

    /* Terminal size in rows and columns (we are obviously in text mode */
    int sz_rows, sz_cols;

    /* Cursor position, 0-indexed */
    int curx, cury;

    /* Display offset in the file, 0-indexed again */
    int offx, offy;

    /* An data structure to store each row */
    rows_t* rows;

#if DEBUG
    FILE* debug_log;
#endif
} fled_config_t;

typedef enum special_keys {
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY,
    DEL_KEY
} special_keys_t;

/* A struct containing global definitions
 * defined in main.c in a non-extern manner
 * although it is not really used there
 */
extern fled_config_t* E;

#endif
