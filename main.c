#include <ctype.h>
#include <stdio.h>

#include <editor/common.h>
#include <editor/terminal.h>

/* this program uses the termios stuff and
 * requires a unix-like environment to run
 * linux and OSX are okay, but for windows;
 * see http://viewsourcecode.org/snaptoken/kilo/01.setup.html
 */

/*** input ***/

void process_key() {
    char c = readkey();

    if (c == CTRL_KEY('q')) { /* continue while the character is not q */
        exit(0);
    }

    if (isprint(c)) {
        printf("%d ('%c')\r\n", c, c);
    } else if(c) { /* don't print 0 on a timeout */
        printf("%d\r\n", c);
    }
}

/*** init ***/

int main() {
    rawmode();

    while(1) {
        process_key();
    }

    return 0;
}
