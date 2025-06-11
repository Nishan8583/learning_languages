use std::time::Duration;
use tokio::task::spawn_blocking;

async fn hello_delay(task: u64, time: u64) {
    println!("Task {task} has started");
    //std::thread::sleep(Duration::from_millis(time));  // sleeping makes whole system thread to go to sleep, not the tokio task, so runtime itself has slept
    tokio::time::sleep(Duration::from_millis(time)).await;  // with this only task is sleeping, not the thread
    println!("Task {task} has finished");
}


async fn hello_delay_spawn_block(task: u64, time: u64) {
    println!("Task {task} has started");

    // do thjis for tasks that require a lot of compute, don't finish until we finish it\
    // spawns an actual thread, blocking pool keeps hot thread ready to go, so starting a thread becomes less costly
    let _ = spawn_blocking(move || {
        std::thread::sleep(Duration::from_millis(time));  // sleeping makes whole system thread to go to sleep, not the tokio task, so runtime itself has slept
    });
    //tokio::time::sleep(Duration::from_millis(time)).await;  // with this only task is sleeping, not the thread
    println!("Task {task} has finished");
}




#[tokio::main]
async fn main() {
    println!("Hello, world!");
    tokio::join!(
        hello_delay(0, 500),
        hello_delay(1, 1000),
        hello_delay(2, 2000),
        hello_delay(3, 5000),
    );
}
