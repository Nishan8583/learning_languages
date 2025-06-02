/*
1. You can have as many readers as you want, no contention there.
2. To write, request a write lock, which waits for all the readers to finish, and then locks the resource for writing.
*/
use std::{sync::RwLock, thread};
use once_cell::sync::Lazy;  // Initialize during runtime, not compile time

static Users: Lazy<RwLock<Vec<String>>> = Lazy::new(||{
    RwLock::new(build_users())
});  // Lazy initialization of the RwLock, first it will be empty, now when anyone tries to access it it will call 
// the build_users and initialize it;


fn build_users() -> Vec<String> {
    vec![
        "Alice".to_string(),
        "Bob".to_string(),
        "Charlie".to_string(),
    ]
}

fn read_line() -> String {
    let mut input: String = String::new();
    std::io::stdin().read_line(&mut input).unwrap();
    input.trim().to_string()
}

fn main() {
    // we dont have aanything to move so nothing to move

    // background thread
    std::thread::spawn(||{
        loop {
            println!("Current userss in a thread ");
            let users = Users.read().unwrap();
            println!("{users:?}");
            thread::sleep(std::time::Duration::from_secs(3)); // sleep for 3 seconds
        }
    });

    loop {
        println!("Enter a name to add to user list (q to quit)");
        let input = read_line();
        if input == "q" {
            break;
        }
        let mut lock = Users.write().unwrap(); // acquire a write lock, this will block until all readers are done
        lock.push(input); // push the value into the vector
    }
    println!("Hello, world!");
    
}
