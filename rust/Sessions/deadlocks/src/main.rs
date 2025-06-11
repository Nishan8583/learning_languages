use std::sync::Mutex;


static VALUE: Mutex<u32> = Mutex::new(0);

fn main() {

    let lock  = VALUE.lock().unwrap(); // Acquire the lock

    // Rust compiler will not detect this deadlock
    //let lock = VALUE.lock().unwrap(); // This will block, causing a deadlock

    // Try to detect lock with try_lock()
    if let Ok(lock) = VALUE.try_lock() {
        println!("Lock acquired: {}", *lock);
    } else {
        println!("Failed to acquire lock");
    }

    println!("Hello, world!");
}
