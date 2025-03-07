use std::collections::HashMap;

use mini_redis::{Command, Connection, Frame};
use tokio::net::{TcpListener, TcpStream};

#[tokio::main]
async fn main() {
    let listener = TcpListener::bind("127.0.0.1:6379")
        .await
        .expect("Could not bind to address");

    loop {
        let (socket, _) = listener
            .accept()
            .await
            .expect("could not accept new socket stream");

        // if i used the line below, the code would wait until process is done
        // process(socket).await;
        //
        //

        // spawns a new Task, the tokio run time then may execute the task in the same thread or a
        // new one
        // We use move because the lifetime of Task is `static, i.e. it must not contained any idea
        // that is owned by someone else
        tokio::spawn(async move {
            if let Err(e) = process(socket).await {
                println!("error while executing process task {:?}", e);
            }
        });
    }
}

async fn process(stream: TcpStream) -> Result<(), String> {
    use mini_redis::Command::{Get, Set};

    let mut db = HashMap::new();
    // mini_redis specific stuff, with connection we can read and write frames for mini mini_redis
    let mut connection = Connection::new(stream);

    if let Some(frame) = connection
        .read_frame()
        .await
        .expect("error while reading frame")
    {
        let response = match Command::from_frame(frame).map_err(|_| String::from("some error"))? {
            Set(cmd) => {
                db.insert(cmd.key().to_string(), cmd.value().to_vec());
                Frame::Simple("OK".to_string())
            }
            Get(cmd) => {
                if let Some(v) = db.get(cmd.key()) {
                    Frame::Bulk(v.clone().into())
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
