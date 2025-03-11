
use std::{collections::HashMap, sync::{Mutex,Arc}};

use bytes::Bytes;
use mini_redis::{Command, Connection, Frame};
use tokio::net::{TcpListener, TcpStream};

type DB = Arc<Mutex<HashMap<String,Bytes>>>;

#[tokio::main]
async fn main() {
    let listener = TcpListener::bind("127.0.0.1:6379")
        .await
        .expect("Could not bind to address");

    let db = Arc::new(Mutex::new(HashMap::new()));

    loop {
        let (socket, _) = listener
            .accept()
            .await
            .expect("could not accept new socket stream");

        // if i used the line below, the code would wait until process is done
        // process(socket).await;
        //
        //
        let db_clone = db.clone();
        // spawns a new Task, the tokio run time then may execute the task in the same thread or a
        // new one
        // We use move because the lifetime of Task is `static, i.e. it must not contained any idea
        // that is owned by someone else
        tokio::spawn(async move {
            if let Err(e) = process(socket,db_clone).await {
                println!("error while executing process task {:?}", e);
            }
        });
    }
}

async fn process(stream: TcpStream, db: DB) -> Result<(), String> {
    use mini_redis::Command::{Get, Set};


    // mini_redis specific stuff, with connection we can read and write frames for mini mini_redis
    let mut connection = Connection::new(stream);

    while let Some(frame) = connection
        .read_frame()
        .await
        .expect("error while reading frame")
    {
        let response = match Command::from_frame(frame).map_err(|_| String::from("some error"))? {
            Set(cmd) => {

                let mut db = db.lock().expect("another thread while holding this mutex panicked");// When a lock is contended, the thread executing the task must block and wait on the mutex. This will not only block the current task but it will also block all other tasks
                db.insert(cmd.key().to_string(), cmd.value().clone());
                Frame::Simple("OK".to_string())
            }
            Get(cmd) => {
                let db = db.lock().expect("another thread while holding this mutex panicked");
                if let Some(v) = db.get(cmd.key()) {
                    Frame::Bulk(v.clone())
                } else {
                    Frame::Null
                }
            }
            cmd => {
                println!("Unimplemented {:?}", cmd);
                Frame::Null
            }
        };

        connection
            .write_frame(&response)
            .await
            .map_err(|e| format!("while sending data back to client {:?}", e))?;
    }

    Ok(())
}
