use std::sync::mpsc;

enum Commands {
    SayHello,
    Quit,
}

fn main() {
    let (tx,rx)= mpsc::channel::<Commands>();

    let handle = std::thread::spawn( move || {
        
        // while let will keep running till channel recieves something, when closed, // it will exit the loop
        // rx.recv() when there is no data, it will park, kernel and ocassioanly check if data is there.
        while let Ok(command) = rx.recv() {
            match command {
                Commands::SayHello => println!("Hello from the thread!"),
                Commands::Quit => {
                    println!("Thread is quitting.");
                    break;
                }
            }

        }
    });

    for _ in 0..10 {

        tx.send(Commands::SayHello).expect("Failed to send command");
    }

    tx.send(Commands::Quit).expect("failed to send quit command");
    handle.join().expect("Thread panicked");
    println!("Hello, world!");
}
