use std::pin::Pin;

use async_recursion::*;

fn fibbonaci(n: u32) -> u32 {
    match n {
        0 => 0,
        1 => 1,
        _ => fibbonaci(n-1) + fibbonaci(n-2)
    }
}


// returning normal value in async will not work, because rust can not guarantee the ownership and the execution flow, it has to return a future
// SO compiler will complain to pin this using Box::Future, it is in future crate
// pinning means putting the value behind a pointer, in memoy heap, and we work on it, once the pin goes out of scope the memory is freed
// we could have used Box::Future, but it would be painful, because we woudl need to wrap all the return value inside the Box future initialization
// The async_recursion macro, takes care of that

#[async_recursion]
async fn async_fibbonaci(n: u32) -> u32 {
    match n {
        0 => 0,
        1 => 1,
        _ => async_fibbonaci(n-1).await + async_fibbonaci(n-2).await
    }
}

#[tokio::main]
async fn main() {
    println!("Sync fibbonaci is {}",fibbonaci(10));
    println!("Async fibbonaci recursion value {}",async_fibbonaci(10).await);

    let mut future = async {
        println!("hello from future");
    };

    tokio::pin!(future); // on the next line we can not have a reference to mutable future, unless we pin it like this, this pattern is said to be common while we tokio::select
    (&mut future).await;

}


async fn one() {
    println!("One");
}

async fn two() {
    println!("two")
}

// THe hard way
// Pin becasue we want to pin it in the memory
// Box becase we keep a smart pointer to point to
// dyn becasue we will only the size of the value during runtime
// Future that Unit type as output
// So it becomes Pin the box type of (dynamic Future) into the memory
async fn call_one_of_them_hard_way(n: u32) -> Pin<Box<dyn Future<Output = ()>>> {
    match n {
        1 => Box::pin(one()),
        2 => Box::pin(two()),
        _ => panic!("WHAT THE HELLLL"),
        
    }
}