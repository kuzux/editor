extern crate termion; // terminal control related stuff
mod input; // controller
mod output; // view

mod viewmodel;
mod model;

use std::io::{stdin, stdout, Stdout};

use termion::raw::{IntoRawMode, RawTerminal};

use model::Model;
use viewmodel::ViewModel;

fn main() {
    let mut out : RawTerminal<Stdout> = stdout().into_raw_mode()
        .unwrap();

    let mut inp = stdin();

    let model : Model = Model { };
    let vm : ViewModel = ViewModel { quit: false };

    loop { 
        let (model, vm) = input::handle(&mut inp, model, vm);
        output::draw(&mut out, vm);

        if (vm.quit) {
            break;
        }
    }
}
