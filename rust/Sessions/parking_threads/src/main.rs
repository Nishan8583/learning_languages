use std::time::Duration;


/* 
Parking threads means the thread is suspended, it will stay suspended it recieves a signal to wake up. i.e. another thread unapakrs it.
A thread can only be parked by itself, not by any other thread.
Be careful when u do thing, if u do not unpark it, it will stay suspended forever.
This is useful when you want to wait for a condition to be met before continuing execution.
On some platforms, when u join a parked thread we will get a error, on others it will just keep on running.

This becomes building blocks of threadpool, becasue we start bunch of threads, that are parked, we can unpark it to work, and keep on looping
*/
fn parkable_thread(n: u32) {
    loop {
        std::thread::park(); // This will block the thread until it is unparked
        //std::thread::park_timeout(Duration::from_secs(1)); // This will block the thread for 1 second, then it will be unparked, we use this instead of sleep in cases where we wan the sleep to be interruptible
        println!("Thread {} is unparked briefly", n);
    }
}
fn read_input() -> String {
    let mut buf = String::new();
    std::io::stdin().read_line(&mut buf).expect("Failed to read line");
    buf.trim().to_string()
}
fn main() {
    let mut handles = Vec::new();

    for i in 0..100 {
        let handle = std::thread::spawn(move || {
            parkable_thread(i);
        });
        handles.push(handle);
    }


    println!("Threads started");
    loop {
        println!("Please enter the thread number you want to unpark: ");
        // if using print!() we need to flush the output buffer
        //use std::io::Write;
        //std::io::stdout().flush().expect("Failed to flush stdout");
        let buf = read_input();
        if buf == "q" {
            break;
        }

        let number = buf.parse::<usize>().expect("Invalid input, please enter a number or 'q' to quit");

        if number >= handles.len() {
            println!("Please enter valid number between 0 and {}", handles.len() - 1);
            continue;
        }

        handles[number]
        .thread()  // we can access the inner thread using .thread(), nice to know
        .unpark();
    }
    println!("Hello, world!");
}
