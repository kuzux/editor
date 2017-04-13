use std::io::Stdin;
use std::process;

use termion::event::Key;
use termion::input::TermRead;

use model::Model;
use viewmodel::ViewModel;

pub fn handle(inp: &mut Stdin, model: &mut Model, vm: &mut ViewModel) { 
    match inp.keys().next().unwrap().unwrap() {
        Key::Ctrl('q') => { vm.quit = true; }
        _ => { }
    }
}