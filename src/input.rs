use std::io::stdin;
use std::process;

use termion::event::Key;
use termion::input::TermRead;

pub fn handle() {
    for c in stdin().keys() {
        match c.unwrap() {
            Key::Ctrl('q') => { process::exit(0); }
            _ => { }
        }
    }
}