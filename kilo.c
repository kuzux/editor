/*** includes ***/

#include <unistd.h>
#include <termios.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>

/* this program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/*** data ***/

struct termios orig_term;

/*** terminal ***/

/* exit the program after an error */
void die(const char *s) {
  perror(s);
  exit(1);
}

/* disable terminal raw mode
 * by restoring the terminal attributes we read earlier
 */
void rawmode_disable() {
    int res = tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_term);
    if(res == -1) {
        die("tcsetattr");
    }
}

/* enable the "raw mode" of a console
 * see http://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
 * raw mode = remove all or most benefits a console normally provides fot us
 * we also set a few defaults as well, those should be commonly set as well
 */
void rawmode() {
    struct termios raw;
    int res;

    res = tcgetattr(STDIN_FILENO, &orig_term);

    if(res == -1) {
        die("tcgetattr");
    }

    atexit(rawmode_disable);

    raw = orig_term;

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
        die("tcsetattr");
    }
}

/*** init ***/

int main() {
    rawmode();

    char c;
    int res;
    while(1) {
        c = '\0';

        /* use the read system call directly because we can't read a
         * raw character with any other method.
         */
        res = read(STDIN_FILENO, &c, 1); /* read a character */
        if (c == 'q') { /* continue while the character is not q */
            break;
        }

        /* exit if the read call fails
         * also check for EAGAIN because cygwin does that when the read
         * call times out. errno and EAGAIN are defined in errno.h
         */
        if(res == -1 && errno != EAGAIN) {
            die("read");
        }
    
        if (isprint(c)) {
            printf("%d ('%c')\r\n", c, c);
        } else if(c) { /* don't print 0 on a timeout */
            printf("%d\r\n", c);
        }
    }

    return 0;
}
