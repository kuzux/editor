extern crate termion; // terminal control related stuff
mod input; // controller
mod output; // view

use termion::raw::{IntoRawMode, RawTerminal};
use std::io::{Write, stdout, Stdout};

fn main() {
    let mut out : RawTerminal<Stdout> = stdout().into_raw_mode()
        .unwrap();

    out.write_all(b"asd\r\n")
        .unwrap();

    loop { 
        input::handle();
        output::draw(&mut out);
    }
}
