
fn hello_thread(n: u32) {
    println!("Hello from thread {}", n);
}
fn main() {

    let mut thread_handles = Vec::new();

    for i in 0..20 {
        let handle = std::thread::spawn(move || {hello_thread(i); });  // closures are just inline functions, like when we do error.Go(func(){}), because in spawn, it takes a function that takes only one argument, so we use move to take ownership of i
        // we move the data, because rust memory manager wants thread to own the data
        thread_handles.push(handle);
    }
    println!("Hello, world! from main thread");

    // into_iter() iterats through each element, and consumes the vector, i.e. it takes ownership of the vector and is thus removed
    thread_handles.into_iter().for_each(|handle|{
        handle.join().unwrap();
    });
}
