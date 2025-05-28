
use server::{app::App, auth};

#[tokio::main]
async fn main() -> Result<(),anyhow::Error> {

    let db = auth::new_db();
    let app = App::new_app(db);

    app.listen("0.0.0.0:8080").await?;
    Ok(())
}


