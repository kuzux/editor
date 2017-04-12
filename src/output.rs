use std::io::{Stdout, Write};

use termion;
use termion::raw::RawTerminal;

use viewmodel::ViewModel;

pub fn draw(out : &mut RawTerminal<Stdout>, vm: ViewModel) {
    termion::cursor::Goto(1,1);
    out.write_all(b"asd\r\n")
        .unwrap();
}
