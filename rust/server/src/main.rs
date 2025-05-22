
use server::app::App;

#[tokio::main]
async fn main() -> Result<(),anyhow::Error> {
    let app = App::new_app();

    app.listen("0.0.0.0:8080").await?;
    Ok(())
}


