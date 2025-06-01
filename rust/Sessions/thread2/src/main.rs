fn do_math(n: u32) -> u32 {
    n * n
}

fn main() {

    let mut thread_handles = Vec::new(); // Type is Vec<JoinHandle<u32>> the type is automatically determined by the closure we pass to spawn, which returns a u32

    for i in 0..20 {
        let handle = std::thread::spawn(move || {do_math(i)});  // see how ; is not addedd on do_math, because we want  to reutrn the value
        thread_handles.push(handle);
    }

    println!("Hello, world!");

    thread_handles.into_iter().for_each(|handle|{
        println!("Return value {}",handle.join().unwrap()); // unwrap is used to get the value from the Result type, which is returned by join
    })
}
