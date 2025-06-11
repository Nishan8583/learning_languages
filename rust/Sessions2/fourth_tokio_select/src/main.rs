use std::time::Duration;

async fn do_work() {
    tokio::time::sleep(Duration::from_secs(2)).await;
}

async fn timeout(timeout_t: f32) {
    tokio::time::sleep(Duration::from_secs_f32(timeout_t)).await;
}

#[tokio::main]
async fn main() {
    // tokio select check which future finishes first, after the the others are cancelled
    // Ex: web request, and a timeout, 
    // Ex: multiple web request to multiple DNS servers, which ever finishes first, use that, rest are canceled
    tokio::select! {
        // pattern matching like match, use the value if u want
        _ = do_work() => println!("do work finished first"),
        _ = timeout(1.0) => println!("timeout finsihed first"),
    }
}
