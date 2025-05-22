use std::{collections::HashMap, sync::{Arc, Mutex}};


struct User {
    username: String,
    password: String,
}

pub struct DB {
    userinfo: Arc<Mutex<HashMap<String,User>>>
}


impl DB {

    pub fn new_db() ->Self{
        let db = Arc::new(Mutex::new(HashMap::new()));
        db.lock().unwrap().insert("test".to_string(), User{username:"test".to_string(),password:"test".to_string()});

        DB{
            userinfo: db,
        }

    }

    pub fn check_user(&self,username: &str, password: &str) -> bool {
        println!("checking user {}",username);

        let user_info_guard  =self.userinfo.lock().unwrap();

        let username = username.trim();
        let password = password.trim();

        let user_info = user_info_guard.get(username);
        if user_info.is_none() {
            return false;
        }  

        let user_info = user_info.unwrap();
        

        if user_info.username == username && user_info.password == password  {
            return  true;
        }

        false

    }
}