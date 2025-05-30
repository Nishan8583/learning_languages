
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

async fn spawn_app() {
    let app = App::new_app(new_test_db());
    app.listen("0.0.0.0:8080").await.unwrap();
}


mod tests {
 
    use tokio::io::{AsyncReadExt, AsyncWriteExt};

    use crate::tests::test_client;

    use super::*;

    #[tokio::test]
    async fn test_flow() {

    let app_handle = tokio::spawn(async move {   
        spawn_app().await;
    });

    let mut client1 = test_client::new("test1".to_string(), "password1".to_string());
    client1.connect("127.0.0.1:8080").await.unwrap();
    client1.authetnicatie().await.unwrap();


    let mut client2 = test_client::new("test2".to_string(), "password2".to_string());
    client2.connect("127.0.0.1:8080").await.unwrap();
    client2.authetnicatie().await.unwrap();



    client1.send_message("Hello client 2").await.unwrap();
    let msg = client2.receive_message().await.unwrap();
    assert_eq!(msg, "Hello client 2");


    app_handle.abort();

    }

}
