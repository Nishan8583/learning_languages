
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use thiserror::Error;


pub struct test_client {
    username: String,
    password: String,
    connection: Option<tokio::net::TcpStream>,
}

#[derive(Debug,Error)]
pub enum ClientError {
    #[error("Unexpected first message")]
    UnexpectedFirstMessage,

    #[error("Failed to connect to server: {0}")]
    ConnectionFailed(String),

    #[error("Failed to write to server: {0}")]
    Io(#[from] std::io::Error),  // wiht this #[from] attribute, we can convert std::io::Error into ClientError::Io automatically, so whenever we use the ? operator on a Result that has std::io::Error, it will automatically convert it to ClientError::Io

    #[error("Failed to auithenticate: {0}")]
    AuthenticationFailed(String),

    #[error("Connection not established")]
    ConnectionNotEstablished,
}

impl test_client {
    pub fn new(username:String, password: String) -> Self {
        test_client{
            username,
            password,
            connection: None,
        }
    }

    pub async fn connect(&mut self, addr: &str) -> Result<(), ClientError> {
        let res = tokio::net::TcpStream::connect(addr).await;
        self.connection= match res {
            Ok(mut client) => {
                let mut buf = [0; 2048];
                let n  = client.read(&mut buf).await?;

                let msg = &buf[0..n];

                let msg = std::str::from_utf8(&msg).unwrap();
                if (msg != "Hello client\nPlease enter your Username: ") {
                    return Err(ClientError::UnexpectedFirstMessage);
                };
                Some(client)
            },
            Err(e) => {
                return  Err(ClientError::ConnectionFailed(format!("Failed to connect to {}: {}", addr, e)));
            }
        };
        Ok(())
    }


    pub async fn authetnicatie(&mut self) -> Result<(), ClientError> {
        let client = match self.connection.as_mut() {
            Some(c) => c,
            None => return Err(ClientError::ConnectionNotEstablished),
        };
        
        // send username
        client.write_all(self.username.as_bytes()).await?;

        //read response, we really do not care about the response here, we just want to send the password in the next step
        let mut response = [0; 2048];
        let _ = client.read(&mut response).await?;

        client.write_all(self.password.as_bytes()).await?;

        let mut response = [0; 2048];
        let n = client.read(&mut response).await?;
        if n == 0 {
            return Err(ClientError::AuthenticationFailed("No response from server after sending username and password".to_string()));
        }

        let msg = &response[0..n];
        let msg = std::str::from_utf8(&msg).unwrap();

        if msg != "Authentication successful\n" {
            return Err(ClientError::AuthenticationFailed(format!("Server responded with: {}", msg)));
        }
        
        Ok(())
    }




}

/*
The #[from] attribute tells Rust (via the thiserror crate) to automatically implement From<std::io::Error> for your ClientError::WriteFailed variant.

So, when you use ? on a call like client.write_all(...).await?, and write_all returns a std::io::Error, Rust will automatically convert that std::io::Error into your ClientError::WriteFailed using the generated From implementation.

In summary:

write_all(...).await returns Result<_, std::io::Error>
The ? operator tries to convert std::io::Error into your function's error type (ClientError)
Because of #[from], std::io::Error is automatically converted to ClientError::WriteFailed
This makes error propagation with ? seamless and ergonomic!
This is one of the main benefits of using thiserror and the #[from] attribute in custom error enums.
*/