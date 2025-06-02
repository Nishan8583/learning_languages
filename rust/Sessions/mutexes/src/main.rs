use std::sync::Mutex;


/*
1. Use atomics when u can since those are way faster.
2. For complex types, use Mutexes, use fine grain lokcing over mutexes
3. Mutexes can slow you down, lock and unlock, check if just a sequential for loop is faster, at times it is
*/
static NUMBERS: Mutex<Vec<i32>> = Mutex::new(Vec::new());

fn main() {
    println!("Hello, world!");

    let mut handles = Vec::new();
    for i in 0..10 {
        let handle = std::thread::spawn(||{
            let mut lock = NUMBERS.lock().unwrap();
            lock.push(1);

        });

        handles.push(handle);
    }

    handles
    .into_iter()
    .for_each(|h| {
        h.join().unwrap();
    });

    let numbers = NUMBERS.lock().unwrap();
    println!("Numbers: {:#?}", *numbers);
}
