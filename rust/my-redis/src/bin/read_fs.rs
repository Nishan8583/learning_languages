use tokio::fs::File;
use tokio::io::{self,AsyncReadExt, AsyncWriteExt}; // Contains read() async method for readin, self means we can
// now use io:: 


#[tokio::main]
async fn main() -> io::Result<()> {
    write_to_file().await?;
    read_to_file().await?;
    Ok(())
}

async fn write_to_file() -> io::Result<()> {
    let mut f = File::create("foo.txt").await?;
    f.write_all(b"some bytes").await?;
    Ok(())
}

async fn read_to_file() -> io::Result<()> {

    let mut f = File::open("foo.txt").await?;
    let mut buffer = [0;10];

    let n = f.read(&mut buffer[..]).await?;

    println!("The bytes read {:?}",&buffer[..n]);
    Ok(())

} 
