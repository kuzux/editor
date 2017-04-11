use std::io::{Stdout, Write};

use termion;
use termion::raw::RawTerminal;

pub fn draw(out : &mut RawTerminal<Stdout>) {
    termion::cursor::Goto(1,1);
    out.write_all(b"asd\r\n")
        .unwrap();
}