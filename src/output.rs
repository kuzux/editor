use std::io::{Stdout, Write};

use termion;
use termion::raw::RawTerminal;

use viewmodel::ViewModel;

pub fn draw(out : &mut RawTerminal<Stdout>, vm: &ViewModel) {
    termion::cursor::Goto(1,1);

   let (cols, rows) = termion::terminal_size()
        .unwrap();

    for _ in 1..(rows+1) {
        write!(out, "{}", termion::clear::UntilNewline)
            .unwrap();
        out.write_all(b"~\r\n")
            .unwrap();
    }
}
