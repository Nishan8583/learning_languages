
use std::collections::HashMap;
use std::sync::{Arc, Mutex};

pub struct user {
    username: String,
    password: String,
}
pub struct testDB {
    pub db: Arc<Mutex<HashMap<String, String>>>,
}