use mini_redis::{client, Result};

#[tokio::main]
async fn main() -> Result<()> {
    let mut client = client::connect("127.0.0.1:6379").await?;

    client.set("Hello", "World".into()).await?;

    let result = client.get("Hello").await?;

    if let Some(value) = result {
        println!("Response from server {:?}", value);
    } else {
        println!("Value shoudlb be present but is empty");
    }
    Ok(())
}
