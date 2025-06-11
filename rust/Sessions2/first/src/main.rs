use tokio::runtime;

async fn hello() {
    println!(
        "Hello tokio"
    );
}

async fn hello2() {
    println!("Hello Tokio2");
}

async fn hello_with_result() -> u32 {
    1
}


async fn hello_with_result2() -> String {
    "Hello from return".into()
}


fn example_calling_runtime() {
    let rt = runtime::Builder::new_current_thread()  // this becomes single threaded
    .enable_all()  // to use all features
    .build()
    .unwrap();

    rt.block_on(hello()); // blocks on the function call, it .awaits on the future

    let rt2 = runtime::Builder::new_multi_thread()
    .enable_all()
    .worker_threads(4)  // how many worker threads u want
    .build()
    .unwrap();

    _ = rt2;
}

async fn ticker() {
    for i in 0..10 {
        println!("tick {i}");
    }
}


async fn ticker_with_yield() {
    for i in 0..10 {
        println!("tick with yield {i}");
        tokio::task::yield_now().await; //  i have done enough task, let other taks now do some work;
    }
}
// Using macros
// passing paramters to macros
// #[tokio::main(flavor="current_thread")]

// by default it will be multi threaded
#[tokio::main]
async fn main() {
    hello().await;  // no need to do block_on

    // join similar to join on futures
    tokio::join!(hello(),hello2());  // join executes all the functions and .awaitts on their futures

    let res = tokio::join!(hello_with_result(),hello_with_result2()); // tuple supports different type
    println!("{res:?}");

    // spawns a task, very likely its going to be executed right away, it will not use another thread
    // rather it will use the task pool we already have, like go
    // so we no need to await
    // if we await on it, otherr tasks stop and it will be executed
    // if we do not await on it, it runs anyway, it becomes a sort of background process
    tokio::spawn(ticker());
    hello().await;


    // async is cooperative, i.e. unlike OS threads where each thread might get some time
    // in async runtime, unless u .await it is not going to share the ticker load
    // since ticker is simple, no opportunity to be broken down into threads
    // So if there are no awaits inside the async task, tasks are gonna go sequentially
    // we use tokio::task::yield_now().await; which is a way of saying, i have done enough task, let other taks now do some work;
    // in our code see ticker_with_yield

    // context swithicing between tasks is very fast because it in the same thread

    let _  = tokio::join!(ticker_with_yield(),hello(),ticker_with_yield());
}

