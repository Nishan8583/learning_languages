use std::sync::mpsc;

// FnOnce a type of function it does not state in general
// Send trait means its sendable across threads
// static lifetime
// Type aliasing
type Job = Box<dyn FnOnce() + Send +'static>; // Basically a function that takes no arguments and returns nothing, but can be sent across threads

fn hi_there() {
    println!("Hi trhere !");
}

fn main() {
    let (tx,rx) = mpsc::channel::<Job>();

    let handle = std::thread::spawn(move || {
        while let Ok(job) = rx.recv() {
          job();  
        }
    });

    // A closure functions
    // Each here is a funciton pointer
    let job = || {println!("Hello from the job!");};
    let job2 = || {
        for i in 0..10 {
            println!("Job 2 is running {}", i);
        }
    };

    
    tx.send(Box::new(job)).expect("msg failed to send");
    tx.send(Box::new(job2)).expect("msg failed to send");
    tx.send(Box::new(hi_there)).expect("msg failed to send");
    tx.send(Box::new(||{println!("I'm a box")})).expect("msg failed to send");

    drop(tx);
    handle.join().expect("Thread panicked");
    println!("Hello, world!");
}
