use std::sync::{Arc,Mutex};

use crate::{auth, client};

pub struct App {
    db: Arc<Mutex<auth::DB>>, // Using Arc and Mutex to allow shared mutable access to the database, database will have info about users and their passwords

}

impl App {

    pub fn new_app() -> Self {
        App {
            db: Arc::new(Mutex::new(auth::DB::new_db())),
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