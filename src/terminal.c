#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdio.h>

#include <editor/common.h>
#include <editor/terminal.h>

/* enable the "raw mode" of a console
 * see http://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 * raw mode = remove all or most benefits a console normally provides fot us
 * we also set a few defaults as well, those should be commonly set as well
 */

/* disable terminal raw mode
 * by restoring the terminal attributes we read earlier
 */
void rawmode_disable() {
    int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &E->orig_term);
    if(res == -1) {
        DIE("tcsetattr");
    }
}

void rawmode() {
    struct termios raw;
    int res;

    res = tcgetattr(STDIN_FILENO, &E->orig_term);

    /* bail if the call fails
       die is a utility macro defined in editor/common.h */
    if(res == -1) {
        DIE("tcgetattr");
    }

    atexit(rawmode_disable);

    raw = E->orig_term;

    /* we disable several flags in the termios structure's flag fields
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
  
    /* bitmask defined in termios.h 
     * so that each character is 8 bit 
     */
    raw.c_cflag |= (CS8);

    /* set the read timeout to 100ms 
     * if no input is given */
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    if(res == -1) {
        DIE("tcsetattr");
    }
}

/* read a single keypress via the raw mode */
char readkey() {
    char c;
    int res;

    c = '\0';

    /* use the read system call directly because we can't read a
     * raw character with any other method.
     */
    res = read(STDIN_FILENO, &c, 1); /* read a character */

    /* exit if the read call fails
     * also check for EAGAIN because cygwin does that when the read
     * call times out. errno and EAGAIN are defined in errno.h
     */
    if(res == -1 && errno != EAGAIN) {
        DIE("read");
    }

    return c;
}

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
    printf("\r\n&buf[1]: '%s'\r\n", &buf[1]);
    
    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2) return -1;
    return 0;
}

/* read the window size (in rows and columns) into
 * the two arguments that are provided.
 * Return value is 0 on success and -1 on failure.
 */
int get_ws(int* rows, int* cols) {
    /* try to find the terminal window size
     * the ioctl function and the TIOCGWINSZ macro is defined in sys/ioctl.h
     */

    struct winsize ws;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        /* fallback method to get the screen size
         * see http://viewsourcecode.org/snaptoken/kilo/03.rawInputAndOutput.html
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
