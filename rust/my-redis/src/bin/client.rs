use std::collections::btree_map::Keys;

use mini_redis::client;
use tokio::sync::{mpsc, oneshot};

use bytes::{Bytes, BytesMut};


#[derive(Debug)]
enum Command {
    Get {
        key: String,
        resp: Responder<Option<Bytes>>,
    },
    Set {
        key: String,
        value: Bytes,
        resp: Responder<()>,
    }
}

type Responder<T> = oneshot::Sender<mini_redis::Result<T>>;

#[tokio::main]
async fn main() {
    let mut client = client::connect("127.0.0.1:6379").await.expect("could not connect to the server");

    let (tx, mut rx) = mpsc::channel(32); // create a channel of multiple producer, and single
    // consumer with max capacity fo 32
    //
    

    let tx2 = tx.clone();
    let t1 = tokio::spawn(async move {

    let (resp_tx,resp_rx) = oneshot::channel();
        let cmd = Command::Get { key: "foo".to_string(), resp: resp_tx };
        tx.send(cmd).await.unwrap();

        let res = resp_rx.await;
        println!("Got = {:?}",res);
    });

    let t2 = tokio::spawn(async move {

    let (resp_tx,resp_rx) = oneshot::channel();
        let cmd = Command::Set { 
            key: "foo".to_string(), 
            value: "Bar".into(), 
            resp: resp_tx,
        };
        tx2.send(cmd).await.unwrap();

        let res = resp_rx.await;
        println!("GOT = {:?}",res);
    });

    let manager = tokio::spawn( async move {while let Some(cmd) = rx.recv().await {
        match cmd {
            Command::Get { key,resp:resp_rx } => {
                let resp =  client.get(&key).await;
                if let Err(e) = resp_rx.send(resp) {
                    println!("erorr while sending to oneshot get receiver {:?}",e);
                }
            },
            Command::Set { key, value,resp:resp_rx } => {

                let  resp  = client.set(&key, value).await;
                if let Err(e) = resp_rx.send(resp) {
                    println!("error while sneding to oneshot set receiver {:?}",e);
                }

                
            }
        }
    }});

    t1.await.unwrap();
    t2.await.unwrap();
    manager.await.unwrap();

    
}
