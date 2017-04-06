#include <ctype.h>

#include <editor/common.h>
#include <editor/terminal.h>
#include <editor/input.h>

void process_key() {
    char c = readkey();

    if (c == CTRL_KEY('q')) { /* continue while the character is not q */
        EXIT();
    }
}
