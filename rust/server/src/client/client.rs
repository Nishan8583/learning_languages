use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpStream;
use std::sync::{Arc, Mutex};

use crate::auth;

pub async  fn handle_client<D: auth::AuthDB+'static >(mut client: TcpStream, db: Arc<Mutex<D>>) {
    let msg = "Hello client\nPlease enter your Username: ";
     if let Err(e) = client.write_all(msg.as_bytes()).await {
        println!("Error while writing to client {}",e);
     }

     let mut username = [0;2046];
     

   let read_bytes = match client.read(&mut username).await {
      Err(e) => {
         println!("Error while reading userame from client {}",e);
         return;
      },
      Ok(bytes) => bytes,
     };
         

     let username  = std::str::from_utf8(&username[0..read_bytes]).unwrap().trim();

     let new_msg = format!("Hey {}Please enter your passwrd: ",username);

     if let Err(e) = client.write_all(new_msg.as_bytes()).await {
      eprintln!("Error when trying to read the password {}",e);
      return ;
     }

     let mut password = [0;2046];

     let n = match client.read(&mut password).await {
      Err(e) => {
         eprintln!("Error when readnig password {}",e);
         return;
      },
      Ok(bytes) => bytes,
     };

     let password = std::str::from_utf8(&password[0..n]).unwrap();
     println!("Username = {} password = {}",username,password);

     if db.lock().unwrap().check_user(username, password) {
      println!("User {} authenticated",username);
      let msg = "Authentication successful\n";

      if let Err(e) = client.write_all(msg.as_bytes()).await {
         eprintln!("Error when writing to client {}",e);
      }
     } else {
      eprintln!("Authentication failed for user {}",username);
     }
}