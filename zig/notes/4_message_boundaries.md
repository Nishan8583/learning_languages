# Message Boundaries

`From: https://www.openmymind.net/TCP-Server-In-Zig-Part-2-Message-Boundaries/`

`
The problem is that this isn't going to work. Actually, the real problem is that something like this might work fairly reliably if you're testing locally, making you think that it's working, but it will definitely not work reliably in a production environment.

It's tempting to think that if the sender does 1 write the receiver has to do 1 read. But TCP is just a continuous stream of bytes, it has no concept of message boundaries. It's possible, and quite common, that a single write will require multiple reads. In fact, if you write a 100 byte message, the receiver might need to do anywhere from 1 to 100 reads. Conversely, if you issue 2 or more writes, the other side might get all that data in a single read.

There's no getting around this, so when we write a message, we need to add our own message boundaries which allows the receiving end to piece the parts together whether it takes 1, 2 or 100 reads.
`

- Use delimeter
- Binary encoding
- Prefix, send size first, in a 32bit value for example, see example of size [here](employee_db/src/server_posix_read_and_write.zig)
- For endinanness info look at [here](3_endianness.md)

