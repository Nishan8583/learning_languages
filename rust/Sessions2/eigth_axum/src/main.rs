use std::net::SocketAddr;
use axum::response::Html;
use axum::routing::post;
use axum::{routing::get, Router};
use serde::Serialize;

#[derive(Serialize)]
struct HelloJson {
    msg: String
}

#[tokio::main]
async fn main() {
    let app = Router::new()
    .route("/", get(say_hello))
    .route("/index.html", get(say_hello_html))
    .route("/post",post(say_hello))
    .route("/index.json",get(hello_json));

let listener = tokio::net::TcpListener::bind("0.0.0.0:3000").await.unwrap();

    axum::serve(listener,app).await.unwrap();
    println!("Hello, world!");
}

async fn say_hello() -> &'static str {
    "Hello World !"
}

async fn say_hello_html() -> Html<&'static str> {
    const content: &str = include_str!("./hello.html");  // a helper macro that reads the file and puts the content in that variable
    Html(content)
    //Html("<h1>Hello World</h1>")
}

async fn hello_json() -> axum::Json<HelloJson>{
    let message= HelloJson{
        msg: String::from("Hello from json")
    };
    axum::Json(message)
    
}