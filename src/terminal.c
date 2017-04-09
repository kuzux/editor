#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <fled/common.h>
#include <fled/terminal.h>

/**
 * TODO: Abstract the terminal functions away into a bunch of separate 
 * callbacks so that we can run in a headless mode as well
 * Another idea: provide an ncurses implementation a well 
 */

/**
 * Disable terminal raw mode
 * by restoring the terminal attributes we read earlier
 */
void rawmode_disable() {
    int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &EF->orig_term);
    if(res == -1) {
        DIE("tcsetattr");
    }
}

/**
 * Enable the "raw mode" of a console
 * see http://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 * raw mode = remove all or most benefits a console normally provides fot us
 * we also set a few defaults as well, those should be commonly set as well
 */
void rawmode() {
    struct termios raw;
    int res;

    res = tcgetattr(STDIN_FILENO, &EF->orig_term);

    /**
     * Bail if the call fails
     * die is a utility macro defined in editor/common.h 
     */
    if(res == -1) {
        DIE("tcgetattr");
    }

    atexit(rawmode_disable);

    raw = EF->orig_term;

    /**
     * We disable several flags in the termios structure's flag fields
     * using the bitwise masks defined in termios.h
     * here's a list of what they do:
     * ECHO : print out the pressed characters
     * ICANON : read each keypress immediately, not after enter is pressed
     * ISIG : disable ctrl+c and ctrl+z to send terminate/sleep signals
     * IXON : disable ctrl+s and ctrl+q
     * IEXTEN : disable ctrl+s and ctrl+o
     * ICRNL : disable ctrl+m 
     * OPOST : turn off auto newline translation
     */

    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
  
    /**
     * A bitmask defined in termios.h 
     * so that each character is 8 bit 
     */
    raw.c_cflag |= (CS8);

    /**
     * Set the read timeout to 100ms 
     * if no input is given 
     */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    if(res == -1) {
        DIE("tcsetattr");
    }
}

/* Read a single keypress from the terminal via the raw mode */
int readkey() {
    char c;
    int res;

    c = '\0';

    /**
     * Use the read system call directly because we can't read a
     * raw character with any other method.
     */
    res = read(STDIN_FILENO, &c, 1); /* read a character */

    /**
     * Exit if the read call fails
     * also check for EAGAIN because cygwin does that when the read
     * call times out. errno and EAGAIN are defined in errno.h
     */
    if(res == -1 && errno != EAGAIN) {
        DIE("read");
    }

    if (c == '\x1b') {
        char seq[3];

        /* Return a single escape character if the input ends there */
        if (read(STDIN_FILENO, &seq[0], 1) != 1) {
            return '\x1b';
        }

        if (read(STDIN_FILENO, &seq[1], 1) != 1) {
            return '\x1b';
        }

        /**
         * Recognized escape sequences:
         * \x1b[A = up arrow
         * \x1b[B = down arrow
         * \x1b[C = right arrow
         * \x1b[D = left arrow
         * \x1b[5~ = page up
         * \x1b[6~ = page down
         * \x1b[1~ = home
         * \x1b[7~ = home
         * \x1b[4~ = end
         * \x1b[8~ = end
         * \x1b[3~ = delete
         */

        /* Try to recognize the escape sequences above */
        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) { 
                    return '\x1b';
                }
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': return HOME_KEY;
                        case '4': return END_KEY;

                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;

                        case '7': return HOME_KEY;
                        case '8': return END_KEY;

                        case '3': return DEL_KEY;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                }
            }
        }

        /**
         * Return a single escape character if the we have an unknown 
         * escape sequence 
         */
        return '\x1b';
    } else {
        return c;
    }
}

/**
 * This function is used if the ioctl call fails.
 * It sets the cursor position to (999, 999) which will hopefully
 * fall outside the terminal window and the terminal (or the
 * terminal emulator) will generate an error in the form of an escape code
 * that contains the number of rows and columns in the terminal.
 * But the contents pretty much are black escape code magic that I don't
 * understand at all.
 */
int get_ws_fallback(int* rows, int* cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4){
        return -1;
    }

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';
    
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
    return 0;
}

/**
 * Read the window size (in rows and columns) into
 * the two arguments that are provided.
 * Return value is 0 on success and -1 on failure.
 */
int get_ws(int* rows, int* cols) {
    /**
     * Try to find the terminal window size
     * the ioctl function and the TIOCGWINSZ macro is defined in sys/ioctl.h
     */
    struct winsize ws;

    /* Try the 'easy' method first - the ioctl function */
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        /**
         * If that fails, the fallback method to get the screen size
         * see http://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html
         * for a more detailed explanation
         */
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) {
            return -1;
        }
        return get_ws_fallback(rows, cols);
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
