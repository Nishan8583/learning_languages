use std::sync::{Arc,Mutex};

use crate::{auth, client};

// Add a generic type so that it becomes easier for testing, also give it 'static lifetime because since we are spawnning a thread
// the tokio::spawn requires everything to have 'static lifetime to prevent dangling references
/*
Copilot response:
Yes, adding the 'static bound to your generic type or trait object is the recommended and idiomatic way in Rust when you need to move data into a spawned async task (like with tokio::spawn). This is required for safety and is the standard approach in async Rust.

Alternatives?

If you do not spawn tasks or threads, you do not need 'static.
If you want to avoid 'static, you would have to avoid moving the data into a spawned task, which is usually not practical for servers.
Using trait objects (e.g., Arc<dyn AuthDB + Send + Sync>) still requires 'static if moved into a spawned task.
Summary:

Adding 'static is the recommended and safe way for async/multithreaded code in Rust.
This is the standard pattern in the Rust async ecosystem.
If you are writing a server or any code that spawns tasks, this is the correct approach
*/
pub struct App<D: auth::AuthDB + 'static> {
    db: Arc<Mutex<D>>, // Using Arc and Mutex to allow shared mutable access to the database, database will have info about users and their passwords

}

impl<D: auth::AuthDB> App<D> {

    pub fn new_app(db: D) -> Self {
        App {
            db: Arc::new(Mutex::new(db)),
        }
    }

    pub async fn listen(&self, address: &str) -> Result<(),anyhow::Error>{

        println!("Listening on {}", address);

        // Creating a TCP listener
        let listener = tokio::net::TcpListener::bind(address).await?;

        loop {
            let (stream,_) = listener.accept().await?;

            let db = self.db.clone();

            tokio::spawn(async move {
                client::handle_client(stream,db).await;
            });
        }

    }

}