use std::io::Stdin;
use std::process;

use termion::event::Key;
use termion::input::TermRead;

use model::Model;
use viewmodel::ViewModel;

pub fn handle(inp: &mut Stdin, model: Model, vm: ViewModel) -> (Model, ViewModel) { 
    for c in inp.keys() {
        match c.unwrap() {
            Key::Ctrl('q') => { return ( model, ViewModel {quit: true, .. vm} ); }
            _ => { }
        }
    }

    (model, vm)
}