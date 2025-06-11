use std::{collections::btree_map::Values, sync::Mutex};


static VALUE: Mutex<u32> = Mutex::new(0);

fn posioner() {
    let mut lock = VALUE.lock().unwrap(); // Acquire the lock
    *lock += 1; // Increment the value
    panic!("Simulating a panic to poison the lock");
}
fn main() {
    let handle = std::thread::spawn(posioner);

    // print the error
    println!("{:?}",handle.join());

    let lock = VALUE.lock(); // This will block until the lock is available
    println!("Lock value: {:?}", lock); // Print the value of the lock


    // recover the data that was inside the poisioned lock
    let recovered_data = VALUE.lock().unwrap_or_else(|e| {
        e.into_inner()
    });
    println!("Recovered data: {:?}", *recovered_data); // Print the recovered data
}
