use futures::TryStreamExt;
use sqlx::{sqlite::SqliteStatement, FromRow, Row};

#[derive(Debug,FromRow)]
struct Message {
    id: i64,
    message: String, //names has to be same as columns name
}

#[tokio::main]
async fn main() -> anyhow::Result<()>{

    // onverts .env into environment variables
    dotenv::dotenv()?;

    let db_url = std::env::var("DATABASE_URL")?;
    
    // pool is just like a ARc, clone it and use it
    let pool = sqlx::SqlitePool::connect(&db_url).await?;

    // Run migrations inside code
    sqlx::migrate!("./migrations")
    .run(&pool)
    .await?;

    // the verbose way of getting rows
    /*
    let messages = sqlx::query("SELECT id,message FROM messages")
    .map(|row: sqlx::sqlite::SqliteRow| {
        let id: i64 = row.get(0); // id is the first row
        let message: String = row.get(1);
        (id,message)
    })
    .fetch_all(&pool).await?;


    for (id, message) in messages {
        println!("{id}: {message}");
    }
    */


    // it will fettch everything, not awaiting, so no yielding
    let message = sqlx::query_as::<_,Message>("SELECT id, message FROM messages")
    .fetch_all(&pool)
    .await?;
    println!("{message:?}");

    update_message(3, "updated message", &pool).await?;

    // as mutable streams, awaits for each row, good for mutliple tasks tokio runtime
    let mut message_stream = sqlx::query_as::<_,Message>("SELECT id, message FROM messages")
    .fetch(&pool);

    while let Some(msg) = message_stream.try_next().await? {
        println!("{msg:?}");
    }


    Ok(())
}


async fn update_message(id: i64, message: &str, pool: &sqlx::SqlitePool) -> anyhow::Result<()>{

    sqlx::query("UPDATE messages SET message=? WHERE id=?")
        .bind(message)  // bind placeholders (?) (prevents sql injection) into a value, should be in order
        .bind(id)
        .execute(pool)
        .await?;

    Ok(())
}