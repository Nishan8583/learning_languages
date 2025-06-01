use std::thread;


fn named_thread() {
    println!("Hello from {}",
        thread::current().name().unwrap_or("<Unable to find thread name>"),
);
}
fn main() {

    let h = thread::Builder::new()
        .name("Thread 1".to_string())
        .stack_size(std::mem::size_of::<usize>()*4) // get size of usize times 4, we can set the stack size of the thread, reduce memory footprint, and may improve thread spawn performance, i.e.e make thread spawn faster
        // if we are spawnning thousands of thread, better to use async
        .spawn(named_thread)  // since a function that takes no arguments is a function that returns (), we can use it directly, without a closure
         .unwrap();
    h.join().unwrap();
}
