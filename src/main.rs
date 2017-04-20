extern crate termion; // terminal control related stuff
mod input; // controller
mod output; // view

mod viewmodel;
mod model;
mod terminal;

use std::io::{stdin, stdout, Stdout, Write};
use std::thread;
use std::time::Duration;

use termion::raw::{IntoRawMode, RawTerminal};

use model::Model;
use viewmodel::ViewModel;

fn main() {
    let mut out = stdout()
        .into_raw_mode()
        .unwrap();

    terminal::set_refresh(&mut out);


    let mut model : Model = Model { };
    let mut vm : ViewModel = ViewModel { quit: false };

    thread::spawn(|| { input::read(&mut stdin()); });

    loop { 
        input::handle(&mut stdin(), &mut model, &mut vm);
        output::draw(&mut out, &vm);

        if vm.quit {
            break;
        }

        out.flush().unwrap();

        thread::sleep(Duration::from_millis(50));
    }
}
