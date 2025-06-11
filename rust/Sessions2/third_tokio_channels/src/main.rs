use std::{sync::mpsc, time::Duration};

enum Command {
    Message(String),
    Quit,
}

#[tokio::main]
async fn main() {
    let (tx, rx) = mpsc::channel::<Command>();
    let (tx_reply,mut rx_reply) = tokio::sync::mpsc::channel::<String>(10); // tokios channel is buffered

    // we can not run async code inside normal sync code, so what we do here is get a handle to the current async runtime
    // and use that to run a sync code inside that handle
    let handle = tokio::runtime::Handle::current();


    // single receiveer here, so move it, can not clone
    std::thread::spawn(move || {
        while let Ok(v) = rx.recv() {
            match v {
                Command::Message(v) => {
                    println!("Standard thread received message {v}");
                    let tx_clone = tx_reply.clone();
                    
                    // we are using the tokio runtime handle to execute async code inside the async runtime
                    handle.spawn(async move {
                        tx_clone.send(v).await.unwrap();
                    });

                   // println!("message is {v}")

                
                },
                Command::Quit => {return;}
            }

        }
    });


    tokio::spawn(async move {
        while let Some(v) = rx_reply.recv().await {
            println!("Inside the tokio async receiver {v}");
        }
    });
    // this is a async main function so we can do a tokio::sleep()
    let mut counter = 0; 
    loop {
        tokio::time::sleep(Duration::from_secs(1)).await;

        // while it is okay to call tx.Send() on a async code using std channels,
        // It is not ok to recieve on standard channels inside aync, because it will block the entire thread
        // so use tokio::mpsc
        tx.send(Command::Message(String::from(format!("Hello from {counter}")))).unwrap();
        counter += 1;
    }
}
