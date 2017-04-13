extern crate libc;

use self::libc::c_int;
use std::mem;
use std::io::Stdout;

use termion::raw::RawTerminal;

use self::libc::termios as Termios;

const VMIN : usize = 6;
const VTIME : usize = 5;
const TCSANOW : c_int = 0;

extern "C" {
    fn tcgetattr(fd: c_int, termptr: *mut Termios) -> c_int;
    fn tcsetattr(fd: c_int, opt: c_int, termptr: *mut Termios) -> c_int;
}

fn get_terminal_attr() -> (Termios, c_int) {
    unsafe {
        let mut ios : Termios = mem::zeroed();
        let attr = tcgetattr(0, &mut ios);
        (ios, attr)
    }
}

fn set_terminal_attr(ios: *mut Termios) -> c_int {
    unsafe { tcsetattr(0, TCSANOW, ios) }
}

pub fn set_refresh(out: &mut RawTerminal<Stdout>) {
    let mut ios : Termios = get_terminal_attr().0;
    ios.c_cc[VMIN] = 0;
    ios.c_cc[VTIME] = 1;
    set_terminal_attr(&mut ios as *mut _);
}