
use std::collections::HashMap;
use std::sync::{Arc, Mutex};
use crate::auth::AuthDB;
use crate::app::App;

pub struct User {
    username: String,
    password: String,
}
pub struct TestDB {
    pub db: Arc<Mutex<HashMap<String, User>>>,
}

fn new_test_db()-> TestDB{
    let mut db: HashMap<String, User> = HashMap::new();
    db.insert(String::from("test1"), User{
        username: String::from("test1"),
        password: String::from("password1"),
    });

    db.insert(String::from("test2"), User{
        username: String::from("test2"),
        password: String::from("password2"),
    });

    TestDB { db: Arc::new(Mutex::new(db)) }
}

impl AuthDB for TestDB {
    fn check_user(&self, username: &str, password: &str) -> bool {
        let user_info_guard = self.db.lock().unwrap(); // it will only crash if the lock is poisoned, which is not expected in tests, poisioned means that a thread panicked while holding the lock

        let username = username.trim();
        let password = password.trim();

        let user_info = user_info_guard.get(username);

        if user_info.is_none() {
            eprintln!("User {} not found in the database", username);
            return false;
        }

        let user  = user_info.unwrap();
        if user.username != username && user.password != password {
            eprintln!("Username password did not match expected username = |{}| password = |{}| got username = |{}| password=|{}|", user.username, user.password,username, password);
            return false;
        }

        return true;

    }
}

async fn test_app() {
    let app = App::new_app(new_test_db());
    app.listen("0.0.0.0:8080").await.unwrap();
}


mod tests {
 
    use tokio::io::{AsyncReadExt, AsyncWriteExt};

    use super::*;

    #[tokio::test]
    async fn test_flow() {

    let app_handle = tokio::spawn(async move {   
        test_app().await;
    });


    let mut client = tokio::net::TcpStream::connect("127.0.0.1:8080").await.unwrap();
    
    let mut buf = [0; 2048];
    let n  = client.read(&mut buf).await.unwrap();

    let msg = &buf[0..n];

    let msg = std::str::from_utf8(&msg).unwrap();
    assert_eq!(msg, "Hello client\nPlease enter your Username: ");

    client.write_all("test1".as_bytes()).await.unwrap();

    // now get next message and send password
    let mut buf = [0; 2048];
    let n = client.read(&mut buf).await.unwrap();

    // server has to send a message to the client to ask for password
    assert_ne!(n, 0);


    // Now server will be expecting a password
    let password = "password1";
    client.write_all(&password.as_bytes()).await.unwrap();


    let mut buf = [0; 2048];
    let n = client.read(&mut buf).await.unwrap();
    assert_ne!(n, 0); // "Authentication successful\n" is 27 bytes long

    let msg = &buf[0..n];
    let msg = std::str::from_utf8(&msg).unwrap();

    assert_eq!(msg, "Authentication successful\n");



    // check if server gives OK response


    app_handle.abort();

    }

}
